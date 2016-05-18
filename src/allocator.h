#pragma once

#include "util/count_map.h"
#include "manager.h"

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
    static GC_allocator& instance() noexcept
    {
        static GC_allocator instance_;
        return instance_;
    }

private:
    friend class traced_ptr<T>;

    std::allocator<T> allocator_;
    double phi_ = initial_phi;

    T* objects_;
    size_t capacity_;

    count_map<T*>     roots_;
    std::vector<bool> used_;
    std::vector<bool> marked_;

    static constexpr size_t initial_capacity = 1024;
    static constexpr double initial_phi      = 0.75;

    GC_allocator()
            : capacity_{initial_capacity}
            , objects_{allocator_.allocate(initial_capacity)}
            , used_(initial_capacity, false)
            , marked_(initial_capacity, false)
    {
        Allocator_manager::instance().register_action(&sweep_);
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

    void inc_(T* ptr)
    {
        if (ptr != nullptr) roots_.inc(ptr);
    }

    void dec_(T* ptr)
    {
        if (ptr != nullptr) roots_.dec(ptr);
    }

    static void mark_recursively_(T* ptr) {
        if (ptr != nullptr) {
            auto inst  = instance();
            auto index = ptr - inst.objects_;

            if (!inst.marked_[index]) {
                inst.marked_[index] = true;
                ::gc::internal::trace_(*ptr, [](auto sub_ptr) {
                    mark_recursively_(sub_ptr);
                });
            }
        }
    }

    static void sweep_() {
        instance().do_sweep_();
    }

    void do_sweep_() {
        size_t live = 0;

        for (size_t i = 0; i < capacity_; ++i) {
            if (used_[i] && !marked_[i]) {
                allocator_.destroy(objects_ + i);
                used_[i] = false;
            }

            else if (used_[i]) {
                ++live;
                marked_[i] = false;
            }
        }
    }

    void collect() {
        for (const auto& p : roots_)
            mark_recursively_(p->first);

        Allocator_manager::instance().sweep();
    }

//    void mark_()
//    {
//        for (auto* ptr : objects_)
//            ::gc::internal::trace_(ptr->data_, &mark_tracer_);
//    }
};

} // end namespace gc
