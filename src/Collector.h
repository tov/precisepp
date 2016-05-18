// A Collector<T> manages the pointers of one type, T.
#pragma once

#include "forward.h"
#include "Collector_base.h"
#include "Collector_manager.h"
#include "traced.h"
#include "Traceable.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <unordered_set>

namespace gc {

template <typename T, typename Allocator, typename PAllocator>
class Collector : private internal::Collector_base
{
public:
    static Collector& instance() noexcept
    {
        static Collector instance_;
        return instance_;
    }

private:
    friend class traced_ptr<T, Allocator, PAllocator>;

    using ptr_t = traced<T>*;

    static_assert(std::is_same<traced<T>, typename Allocator::value_type>::value,
                  "Invalid Allocator");
    static_assert(std::is_same<traced<T>*, typename PAllocator::value_type>::value,
                  "Invalid PAllocator");

    Allocator allocator_;

    std::unordered_set<
            ptr_t,
            std::hash<ptr_t>,
            std::equal_to<ptr_t>,
            PAllocator
    > objects_;

    Collector()
    {
        Collector_manager::instance().register_collector_(*this);
    }

    template<typename... Args>
    ptr_t allocate_(Args&& ... args)
    {
        ptr_t result = allocator_.allocate(1);
        ::new(result) traced<T>(std::forward<Args>(args)...);
        objects_.insert(result);
        return result;
    }

    void deallocate_(ptr_t ptr)
    {
        objects_.erase(ptr);
        ptr->~traced<T>();
        allocator_.deallocate(ptr, 1);
    }

    void inc_(ptr_t ptr)
    {
        if (ptr != nullptr) {
            ++ptr->ref_count_;
        }
    }

    void dec_(ptr_t ptr)
    {
        if (ptr != nullptr) {
            --ptr->ref_count_;
        }
    }

    template <typename S>
    static void mark_recursively_(traced<S>* ptr) {
        if (ptr != nullptr && !ptr->mark_) {
            ptr->mark_ = true;
            ::gc::internal::trace_(ptr->object_, [](auto sub_ptr) {
                mark_recursively_(sub_ptr);
            });
        }
    }

    virtual void save_counts_() override {
        for (ptr_t ptr : objects_)
            ptr->root_count_ = ptr->ref_count_;
    }

    virtual void find_roots_() override {
        for (ptr_t ptr : objects_)
            ::gc::internal::trace_(ptr->object_, [](auto sub_ptr) {
                --sub_ptr->root_count_;
            });
    }

    virtual void mark_() override {
        for (ptr_t ptr : objects_)
            if (ptr->root_count_ > 0)
                mark_recursively_(ptr);
    }

    virtual void sweep_() override {
        for (ptr_t ptr : objects_) {
            if (ptr->mark_)
                ptr->mark_ = false;
            else
                deallocate_(ptr);
            }
    }
};

} // end namespace gc
