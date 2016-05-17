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
public:
    static constexpr size_t initial_capacity = 1024;

    // Right now this being public isn't useful.
    GC_allocator(size_t capacity = initial_capacity)
            : capacity_{capacity}
            , objects_{allocator_.allocate(capacity)}
            , used_(capacity, false)
            , marked_(capacity, false)
    { }

    static GC_allocator& instance() noexcept
    {
        static GC_allocator instance_;
        return instance_;
    }

private:
    friend class traced_ptr<T>;

    std::allocator<T> allocator_;

    T* objects_;
    size_t capacity_;

    count_map<T*>     roots_;
    std::vector<bool> used_;
    std::vector<bool> marked_;

    template<typename... Args>
    T* emplace_(Args&& ... args)
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

    static void destroy_(T* ptr)
    {
        if (ptr != nullptr) {
            auto inst  = instance();
            auto index = ptr - inst.objects_;

            if (inst.used_[index]) {
                inst.allocator_.destroy(ptr);
                inst.used_[index] = false;
            }
        }
    }

    static void mark_(T* ptr)
    {
        if (ptr != nullptr)
            instance().marked_[ptr - instance().objects_] = true;
    }

    static void clear_mark_(T* ptr)
    {
        if (ptr != nullptr)
            instance().marked_[ptr - instance().objects_] = false;
    }

    static void inc_(T* ptr)
    {
        if (ptr != nullptr)
            instance().roots_.inc(ptr);
    }

    static void dec_(T* ptr)
    {
        if (ptr != nullptr)
            instance().roots_.dec(ptr);
    }

    void clear_marks_()
    {
        marked_.assign(capacity_, false);
    }

//    void mark_()
//    {
//        for (auto* ptr : objects_)
//            ::gc::internal::trace_(ptr->data_, &mark_tracer_);
//    }
};

} // end namespace gc
