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

    std::unordered_set<
            ptr_t,
            std::hash<ptr_t>,
            std::equal_to<ptr_t>,
            PAllocator
    > objects_;

    TypedSpace(Collector& collector = Collector::instance())
            : collector_{collector}
            , heap_size_{0}
    {
        collector_.register_space_(*this);
    }

    template<typename... Args>
    ptr_t allocate_(Args&& ... args)
    {
        ptr_t result = allocator_.allocate(1);

        try {
            result->initialize_data_(std::forward<Args>(args)...);
        } catch (...) {
            allocator_.deallocate(result, 1);
            throw;
        }

        objects_.insert(result); // What if this fails?
        ++heap_size_;

        return result;
    }

    void deallocate_(ptr_t ptr)
    {
        objects_.erase(ptr);
        --heap_size_;

        ptr->object_().~T();
        allocator_.deallocate(ptr, 1);
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

    virtual void save_counts_() override
    {
        for (ptr_t ptr : objects_)
            ptr->root_count_() = ptr->ref_count_();
    }

    virtual void find_roots_() override
    {
        for (ptr_t ptr : objects_)
            ::gc::internal::trace_(ptr->object_(), [](auto sub_ptr) {
                if (sub_ptr != nullptr)
                    --sub_ptr->root_count_();
            });
    }

    virtual void mark_() override
    {
        for (ptr_t ptr : objects_)
            if (ptr->root_count_() > 0)
                mark_recursively_(ptr);
    }

    virtual void sweep_() override
    {
        for (ptr_t ptr : objects_) {
            if (ptr->mark_)
                ptr->mark_ = false;
            else
                deallocate_(ptr);
            }
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
        return heap_size_;
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
