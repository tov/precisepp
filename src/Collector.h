#pragma once

#include "forward.h"
#include "util/count_map.h"
#include "Collector_base.h"
#include "Collector_manager.h"
#include "Traced.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace gc {

template<typename T>
class Collector : private Collector_base
{
public:
    static Collector& instance() noexcept
    {
        static Collector instance_;
        return instance_;
    }

private:
    friend class traced_ptr<T>;

    std::allocator<Traced<T>> allocator_;

    using ptr_t = Traced<T>*;

    std::unordered_set<ptr_t> objects_;
    count_map<ptr_t>          roots_;

    Collector()
    {
        Collector_manager::instance().register_collector_(*this);
    }

    template<typename... Args>
    ptr_t allocate_(Args&& ... args)
    {
        ptr_t result = allocator_.allocate(1);
        ::new(result) Traced<T>(std::forward<Args>(args)...);
        objects_.insert(result);
        return result;
    }

    void deallocate_(ptr_t ptr)
    {
        objects_.erase(ptr);
        ptr->~Traced<T>();
        allocator_.deallocate(ptr, 1);
    }

    void inc_(ptr_t ptr)
    {
        if (ptr != nullptr) {
            roots_.inc(ptr);
#ifdef DEBUG_REFCOUNTS
            ++ptr->refcount_;
#endif
        }
    }

    void dec_(ptr_t ptr)
    {
        if (ptr != nullptr) {
            roots_.dec(ptr);
#ifdef DEBUG_REFCOUNTS
            --ptr->refcount_;
#endif
        }
    }

    template <typename S>
    static void mark_recursively_(Traced<S>* ptr) {
        if (ptr != nullptr) {
            if (!ptr->mark_) {
                ptr->mark_ = true;
                ::gc::internal::trace_(ptr->object_, [](auto sub_ptr) {
                    mark_recursively_(sub_ptr);
                });
            }
        }
    }

    virtual void mark_() override {
        for (const auto& p : roots_)
            mark_recursively_(p.first);
    }

    virtual void sweep_() override {
        for (ptr_t ptr : objects_) {
            if (!ptr->mark_) {
                deallocate_(ptr);
            }
        }
    }
};

} // end namespace gc
