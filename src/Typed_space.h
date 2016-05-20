// A `Typed_space<T>` manages the pointers of one type, `T`. It implements the
// interface `Space`, which the `Collector` uses to manage it.
#pragma once

#include "forward.h"
#include "Space.h"
#include "Collector.h"
#include "Traced.h"
#include "traced_ptr.h"
#include "Traceable.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <unordered_set>

namespace gc {

static constexpr size_t initial_page_size = 16;
static constexpr double max_live_ratio    = 0.75;

template <typename T, typename Allocator>
class Typed_space : private internal::Space
{
public:
    // Returns the singleton instance for allocating objects of type `T`.
    static Typed_space& instance() noexcept
    {
        static Typed_space instance_;
        return instance_;
    }

    // Allocates an object of type `T` given arguments to forward to its
    // constructor.
    template <typename... Args>
    traced_ptr<T, Allocator>
    allocate(Args&&... args)
    {
        traced_ptr<T, Allocator> result;
        result.ptr_ = allocate_(std::forward<Args>(args)...);
        result.inc_();
        return result;
    };

private:
    // The type of pointer we are managing.
    using ptr_t = Traced<T>*;

    // Produces a friendly error if the given allocator doesn’t actually
    // allocate the right type.
    static_assert(std::is_same<Traced<T>, typename Allocator::value_type>::value,
                  "Invalid Allocator");

    Allocator allocator_;   // For allocating pages of `Traced<T>`s
    Collector& collector_;  // The collector managing this space
    size_t heap_size_;      // The capacity of this space, in objects
    size_t live_size_;      // The number of used slots
    Traced<T>* pages_;      // Linked list of pages to allocate in
    Traced<T>* free_list_;  // Linked list of free object slots
    size_t next_page_size_; // How big the next page should be

    // Constructs a `Typed_space`, which includes registering it with a
    // collector. By default it uses the default (global) collector. (There is
    // currently nothing useful we can do with non-default spaces/collectors.)
    Typed_space(Collector& collector = Collector::instance())
            : collector_{collector}
            , heap_size_{0}
            , live_size_{0}
            , pages_{nullptr}
            , free_list_{nullptr}
            , next_page_size_{initial_page_size}
    {
        collector_.register_space_(*this);
    }

    // Adds a new page: Requests memory for `next_page_size_`
    // objects from the allocator, adds its slots to the free list, and
    // adds the new page to the front of the page list. Doubles the size for
    // next time.
    void add_page_()
    {
        ptr_t page = allocator_.allocate(next_page_size_);
        page[0].initialize_header_(next_page_size_, pages_);
        pages_ = page;

        for (size_t i = 1; i < next_page_size_; ++i)
            add_to_free_list_(&page[i]);

        heap_size_ += next_page_size_ - 1;
        next_page_size_ *= 2;
    }

    // Adds the given pointer to the free list.
    void add_to_free_list_(ptr_t ptr)
    {
        ptr->initialize_free_(free_list_);
        free_list_ = ptr;
    }

    // Allocates and initializes an object, given arguments to forward to its
    // constructor. Looks for a slot on the free list first, and then
    // collects if necessary.
    template<typename... Args>
    ptr_t allocate_(Args&& ... args)
    {
        // If the free list is empty, we either need to create the first page
        // or run the collector. Either way, the free list should no longer
        // be null.
        if (free_list_ == nullptr) {
            if (pages_ == nullptr) {
                add_page_();
            } else {
                collector_.collect();
            }

            if (free_list_ == nullptr)
                throw std::bad_alloc{};
            // Is this the wrong place to do this? Maybe we should be
            // asserting instead, and letting `add_page_` throw if necessary.
        }

        // Grab a slot from the free list.
        ptr_t result = free_list_;
        free_list_   = free_list_->next_free_();

        // Initialize the slot metadata.
        result->initialize_used_();

        // Now try initializing the object. If the constructor throws we put
        // the slot back on the free list and re-throw. (Do we really want to do
        // a try-catch on every allocation? It might be better to a) leak, or
        // b) use a commit protocol that leaves things in a recoverable state?)
        try {
            ::new(&result->object_()) T(std::forward<Args>(args)...);
        } catch (...) {
            add_to_free_list_(result);
            throw;
        }

        // Allocation success!
        ++live_size_;

        return result;
    }

    // Deallocates the pointed-to object, running its destructor and adding
    // its slot to the free list.
    void deallocate_(ptr_t ptr)
    {
        ptr->object_().~T();
        add_to_free_list_(ptr);
        --live_size_;
    }

    // The marking DFS, using `trace_` to find edges.
    template <typename S>
    static void mark_recursively_(Traced<S>* ptr)
    {
        if (ptr != nullptr && !ptr->mark_) {
            ptr->mark_ = true;
            ::gc::internal::trace_(ptr->object_(), [](auto sub_ptr) {
                mark_recursively_(sub_ptr);
            });
        }
    }

    // Calls the given function on each used `Traced<T>*` in the heap.
    template <typename F>
    void for_heap_(F f)
    {
        for (ptr_t page = pages_; page != nullptr; page = page->next_page_())
            for (size_t i = 1; i < page->page_size_(); ++i)
                if (!page[i].free_)
                    f(&page[i]);
    }

    // The remaining member functions are implementations of Space’s pure
    // virtual members.

    //
    // Collection interface — the four phases of collection (see Space.h)
    //

    // GC phase 1: Copies every ref_count_ to root_count_
    virtual void save_counts_() override
    {
        for_heap_([](ptr_t ptr) {
            ptr->root_count_() = ptr->ref_count_();
        });
    }

    // GC phase 2: Decrements root_count_ for every in-edge coming from
    // another Traced object.
    virtual void find_roots_() override
    {
        for_heap_([](ptr_t ptr) {
            ::gc::internal::trace_(ptr->object_(), [](auto sub_ptr) {
                if (sub_ptr != nullptr)
                    --sub_ptr->root_count_();
            });
        });
    }

    // GC phase 3: Marks the live heap via tracing DFS.
    virtual void mark_() override
    {
        for_heap_([](ptr_t ptr) {
            if (ptr->root_count_() > 0)
                mark_recursively_(ptr);
        });
    }

    // GC phase 4: Sweeps away the dead heap, deallocating and resetting marks.
    virtual void sweep_() override
    {
        for_heap_([this](ptr_t ptr) {
            if (ptr->mark_)
                ptr->mark_ = false;
            else
                deallocate_(ptr);
        });

        if (double(live_size_) / heap_size_ > max_live_ratio)
            add_page_();
    }

    //
    // Stats interface – see comments in `Space`
    //

    virtual size_t element_size_() const override
    {
        return sizeof(T);
    }

    virtual size_t capacity_() const override
    {
        return heap_size_;
    }

    virtual size_t used_() const override
    {
        return live_size_;
    }
};

// Allocates an object of type `T` in the default space, given parameters to
// forward to its constructor.
template <typename T,
          typename Allocator  = std::allocator<Traced<T>>,
          typename... Args>
traced_ptr<T, Allocator>
make_traced_in(Typed_space<T, Allocator>& space, Args&&... args)
{
    return space.allocate(std::forward<Args>(args)...);
}

// Allocates an object of type `T` given a space to allocate in and parameters
// to forward to its constructor.
template <typename T,
          typename Allocator  = std::allocator<Traced<T>>,
          typename... Args>
traced_ptr<T, Allocator>
make_traced(Args&&... args)
{
    auto space = Typed_space<T, Allocator>::instance();
    return space.allocate(std::forward<Args>(args)...);
}

} // end namespace gc
