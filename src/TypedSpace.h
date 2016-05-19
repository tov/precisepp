// A Collector<T> manages the pointers of one type, T.
#pragma once

#include "forward.h"
#include "Space.h"
#include "Collector.h"
#include "traced.h"
#include "traced_ptr.h"
#include "Traceable.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <unordered_set>

namespace gc {

template <typename T, typename Allocator, typename PAllocator>
class TypedSpace : private internal::Space
{
public:
    static TypedSpace& instance() noexcept
    {
        static TypedSpace instance_;
        return instance_;
    }

    template <typename... Args>
    traced_ptr<T, Allocator, PAllocator>
    allocate(Args&&... args)
    {
        traced_ptr<T, Allocator, PAllocator> result;
        result.ptr_ = allocate_(std::forward<Args>(args)...);
        result.inc_();
        return result;
    };

private:
    using ptr_t = traced<T>*;

    static_assert(std::is_same<traced<T>, typename Allocator::value_type>::value,
                  "Invalid Allocator");
    static_assert(std::is_same<traced<T>*, typename PAllocator::value_type>::value,
                  "Invalid PAllocator");

    Allocator allocator_;
    Collector& collector_;
    size_t heap_size_;
    size_t live_size_;
    traced<T>* pages_;
    traced<T>* free_list_;

    TypedSpace(Collector& collector = Collector::instance())
            : collector_{collector}
            , heap_size_{0}
            , live_size_{0}
            , pages_{nullptr}
            , free_list_{nullptr}
    {
        collector_.register_space_(*this);
    }

    void add_page_(size_t size)
    {
        ptr_t page = allocator_.allocate(size);
        page[0].initialize_header_(size, pages_);
        pages_ = page;

        for (size_t i = 1; i < size; ++i)
            add_to_free_list_(&page[i]);

        heap_size_ += size - 1;
    }

    void add_to_free_list_(ptr_t ptr)
    {
        ptr->initialize_free_(free_list_);
        free_list_ = ptr;
    }

    static constexpr size_t initial_page_size = 16;

    template<typename... Args>
    ptr_t allocate_(Args&& ... args)
    {
        if (pages_ == nullptr) {
            add_page_(initial_page_size);
        } else if (free_list_ == nullptr) {
            assert(false);
        }

        ptr_t result = free_list_;
        free_list_   = free_list_->next_free_();

        try {
            result->initialize_used_(std::forward<Args>(args)...);
        } catch (...) {
            result->initialize_free_(free_list_);
            free_list_ = result;
            throw;
        }

        ++live_size_;

        return result;
    }

    void deallocate_(ptr_t ptr)
    {
        ptr->object_().~T();
        ptr->initialize_free_(free_list_);
        free_list_ = ptr;

        --live_size_;
    }

    template <typename S>
    static void mark_recursively_(traced<S>* ptr)
    {
        if (ptr != nullptr && !ptr->mark_) {
            ptr->mark_ = true;
            ::gc::internal::trace_(ptr->object_(), [](auto sub_ptr) {
                mark_recursively_(sub_ptr);
            });
        }
    }

    template <typename F>
    void for_heap(F f)
    {
        for (ptr_t page = pages_; page != nullptr; page = page->next_page_())
            for (size_t i = 1; i < page->page_size_(); ++i)
                if (!page[i].free_)
                    f(&page[i]);
    }

    virtual void save_counts_() override
    {
        for_heap([](ptr_t ptr) {
            ptr->root_count_() = ptr->ref_count_();
        });
    }

    virtual void find_roots_() override
    {
        for_heap([](ptr_t ptr) {
            ::gc::internal::trace_(ptr->object_(), [](auto sub_ptr) {
                if (sub_ptr != nullptr)
                    --sub_ptr->root_count_();
            });
        });
    }

    virtual void mark_() override
    {
        for_heap([](ptr_t ptr) {
            if (ptr->root_count_() > 0)
                mark_recursively_(ptr);
        });
    }

    virtual void sweep_() override
    {
        for_heap([this](ptr_t ptr) {
            if (ptr->mark_)
                ptr->mark_ = false;
            else
                deallocate_(ptr);
        });
    }

    virtual size_t element_size_() override
    {
        return sizeof(T);
    }

    virtual size_t capacity_() override
    {
        return heap_size_;
    }

    virtual size_t used_() override
    {
        return live_size_;
    }
};

template <typename T,
          typename Allocator  = std::allocator<traced<T>>,
          typename PAllocator = std::allocator<traced<T>*>,
          typename... Args>
traced_ptr<T, Allocator, PAllocator>
make_traced(Args&&... args)
{
    return TypedSpace<T, Allocator, PAllocator>::instance()
             .allocate(std::forward<Args>(args)...);
}

} // end namespace gc
