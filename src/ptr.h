#pragma once

#include "tracing.h"
#include "stl.h"
#include "util/count_map.h"

#include <utility>
#include <unordered_set>
#include <cassert>
#include <memory>
#include <vector>

namespace gc
{

template <typename T>
class GC_allocator;

template <typename T, typename = Traceable<T>>
class traced_ptr
{
    friend class GC_allocator<T>;
    friend class Traceable<traced_ptr<T>>;

    T* ptr_;

    traced_ptr(T* ptr) : ptr_{ptr}
    {
//        inc_();
    }

//    void inc_()
//    {
//        if (ptr_ != nullptr) ++ptr_->refcount_;
//    }
//
//    void dec_()
//    {
//        if (ptr_ != nullptr) {
//            assert(ptr_->refcount_ > 0);
//            --ptr_->refcount_;
//        }
//    }
//
//    void clear_()
//    {
//        if (ptr_ != nullptr) ptr_->mark_ = false;
//    }
//
//    void mark_recursively_()
//    {
//        if (ptr_ != nullptr && !ptr_->mark_) {
//            ptr_->mark_ = true;
//            ::gc::internal::trace_(ptr_->data_, [](auto ptr) {
//                ptr->mark_recursively_();
//            });
//        }
//    }

public:
    traced_ptr() : ptr_{nullptr}
    { }

    traced_ptr(const traced_ptr& other)
    {
        ptr_ = other.ptr_;
//        inc_();
    }

    traced_ptr(traced_ptr&& other)
    {
        std::swap(ptr_, other.ptr_);
    }

    traced_ptr& operator=(const traced_ptr& other)
    {
//        dec_();
        ptr_ = other.ptr_;
//        inc_();
        return *this;
    }

    traced_ptr& operator=(traced_ptr&& other)
    {
        std::swap(ptr_, other.ptr_);
        return *this;
    }

    ~traced_ptr()
    {
//        dec_();
    }

    T& operator*()
    {
        return *ptr_;
    }

    const T& operator*() const
    {
        return *ptr_;
    }

    T* operator->()
    {
        return ptr_;
    }

    const T* operator->() const
    {
        return ptr_;
    }
};

template <typename T>
DEFINE_TRACEABLE(traced_ptr<T>)
{
    DEFINE_TRACE(const traced_ptr<T>& p)
    {
        tracer(p);
    }
};

template <typename T, typename... Args>
traced_ptr<T> make_traced(Args&&... args)
{
    return GC_allocator<T>::instance()
            .template make_traced<Args...>(std::forward<Args>(args)...);
}

template <typename T>
class GC_allocator
{
public:
    static GC_allocator& instance() noexcept
    {
        static GC_allocator instance_;
        return instance_;
    }

    template <typename... Args>
    traced_ptr<T> make_traced(Args&&... args)
    {
        for (size_t i = 0; i < capacity_; ++i)
        {
            if (used_[i]) continue;
            allocator_.construct(objects_ + i, std::forward<Args>(args)...);
            used_[i] = true;
            return traced_ptr<T>(objects_ + i);
        }

        // out of memory (for now)
        assert(false);
    }

private:
    static constexpr size_t initial_capacity = 1024;

    GC_allocator(size_t capacity = initial_capacity)
        : capacity_{capacity}
        , objects_{allocator_.allocate(capacity)}
        , used_(capacity, false)
        , marked_(capacity, false)
    { }

//    void clear_marks_()
//    {
//        for (auto* ptr : objects_)
//            ptr->clear_();
//    }

//    void mark_()
//    {
//        for (auto* ptr : objects_)
//            ::gc::internal::trace_(ptr->data_, &mark_tracer_);
//    }

    std::allocator<T> allocator_;

    T* objects_;
    size_t capacity_;

    count_map<T*> roots_;
    std::vector<bool> used_;
    std::vector<bool> marked_;
};

} // end namespace gc
