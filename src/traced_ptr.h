// A traced_ptr<T> is a garbage-collected pointer to a T.

#pragma once

#include "forward.h"
#include "Traceable.h"
#include "traced.h"

#include <utility>
#include <unordered_set>
#include <cassert>
#include <memory>
#include <vector>

namespace gc
{

template <typename T, typename Allocator, typename PAllocator>
class traced_ptr
{
public:
    traced_ptr() : ptr_{nullptr}
    { }

    traced_ptr(std::nullptr_t) : traced_ptr{}
    { }

    traced_ptr(const traced_ptr& other)
    {
        ptr_ = other.ptr_;
        inc_();
    }

    traced_ptr(traced_ptr&& other)
    {
        std::swap(ptr_, other.ptr_);
    }

    traced_ptr& operator=(const traced_ptr& other)
    {
        dec_();
        ptr_ = other.ptr_;
        inc_();
        return *this;
    }

    traced_ptr& operator=(traced_ptr&& other)
    {
        std::swap(ptr_, other.ptr_);
        return *this;
    }

    ~traced_ptr()
    {
        dec_();
    }

    T& operator*()
    {
        return ptr_->object_();
    }

    const T& operator*() const
    {
        return ptr_->object_();
    }

    T* operator->()
    {
        return &ptr_->object_();
    }

    const T* operator->() const
    {
        return &ptr_->object_();
    }

private:
    friend class Traceable<traced_ptr>;
    friend class TypedSpace<T, Allocator, PAllocator>;

    traced<T>* ptr_;

    void inc_()
    {
        if (ptr_ != nullptr)
            ++ptr_->ref_count_();
    }

    void dec_()
    {
        if (ptr_ != nullptr)
            --ptr_->ref_count_();
    }
};

template <typename T, typename Allocator, typename PAllocator>
DEFINE_TRACEABLE(traced_ptr<T, Allocator, PAllocator>)
{
    DEFINE_TRACE(const traced_ptr<T, Allocator, PAllocator>& p)
    {
        tracer(p.ptr_);
    }
};

} // end namespace gc
