#pragma once

#include "util/count_map.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace gc {

template<typename T>
class traced_ptr;

template<typename T>
class GC_allocator
{
    friend class traced_ptr<T>;

    std::allocator<T> allocator_;

    T* objects_;
    size_t capacity_;

    count_map<T*>     roots_;
    std::vector<bool> used_;
    std::vector<bool> marked_;

    static constexpr size_t initial_capacity = 1024;

    GC_allocator()
            : capacity_{initial_capacity}
            , objects_{allocator_.allocate(initial_capacity)}
            , used_(initial_capacity, false)
            , marked_(initial_capacity, false)
    { }

    static GC_allocator& instance() noexcept
    {
        static GC_allocator instance_;
        return instance_;
    }

    template<typename... Args>
    T* allocate_(Args&& ... args)
    {
        for (size_t i = 0; i < capacity_; ++i) {
            if (used_[i]) continue;
            allocator_.construct(objects_ + i, std::forward<Args>(args)...);
            used_[i] = true;
            return objects_ + i;
        }

        // out of memory (for now)
        assert(false);
    }

    void destroy_(T* ptr)
    {
        if (ptr != nullptr) {
            auto index = ptr - objects_;

            if (used_[index]) {
                allocator_.destroy(ptr);
                used_[index] = false;
            }
        }
    }

    void mark_(T* ptr)
    {
        if (ptr != nullptr)
            marked_[ptr - instance().objects_] = true;
    }

    void clear_marks_()
    {
        marked_.assign(capacity_, false);
    }

    void inc_(T* ptr)
    {
        if (ptr != nullptr) roots_.inc(ptr);
    }

    void dec_(T* ptr)
    {
        if (ptr != nullptr) roots_.dec(ptr);
    }

//    void mark_()
//    {
//        for (auto* ptr : objects_)
//            ::gc::internal::trace_(ptr->data_, &mark_tracer_);
//    }
};

} // end namespace gc
