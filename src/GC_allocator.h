#pragma once

#include "forward.h"
#include "util/count_map.h"
#include "Allocator_manager.h"
#include "Traced.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace gc {

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

    std::allocator<Traced<T>> allocator_;

    using ptr_t = Traced<T>*;

    std::unordered_set<ptr_t> objects_;
    count_map<ptr_t>          roots_;

    GC_allocator()
    {
        Allocator_manager::instance().register_action(&sweep_);
    }

    template<typename... Args>
    ptr_t allocate_(Args&& ... args)
    {
        ptr_t result = allocator_.allocate(1);
        allocator_.construct(result, std::forward<Args>(args)...);
        objects_.insert(result);
        return result;
    }

    void deallocate_(ptr_t ptr)
    {
        objects_.erase(ptr);
        allocator_.destroy(ptr);
        allocator_.deallocate(ptr, 1);
    }

    void inc_(ptr_t ptr)
    {
        if (ptr != nullptr) roots_.inc(ptr);
    }

    void dec_(ptr_t ptr)
    {
        if (ptr != nullptr) roots_.dec(ptr);
    }

    static void mark_recursively_(ptr_t ptr) {
        if (ptr != nullptr) {
            if (!ptr->mark_) {
                ptr->mark_ = true;
                ::gc::internal::trace_(ptr->object_, [](auto sub_ptr) {
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

        for (ptr_t ptr : objects_) {
            if (!ptr->mark_) {
                deallocate_(ptr);
            }
        }
    }

    static void mark_() {
        instance().do_mark_();
    }

    void do_mark_() {
        for (const auto& p : roots_)
            mark_recursively_(p->first);
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
