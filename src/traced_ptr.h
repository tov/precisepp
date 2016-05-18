#pragma once

#include "forward.h"
#include "Traceable.h"
#include "stl.h"
#include "Collector.h"

#include <utility>
#include <unordered_set>
#include <cassert>
#include <memory>
#include <vector>

namespace gc
{

template <typename T>
class traced_ptr
{
public:
    traced_ptr() : ptr_{nullptr}
    { }

    template <typename... Args>
    traced_ptr(Args&&... args)
            : ptr_{allocator().allocate_(std::forward<Args>(args)...)}
    {
        inc_();
    }

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

private:
    friend class Traceable<traced_ptr>;

    Traced<T>* ptr_;

    static Collector<T>& allocator()
    {
        return Collector<T>::instance();
    }

    void inc_()
    {
        allocator().inc_(ptr_);
    }

    void dec_()
    {
        allocator().dec_(ptr_);
    }

    void mark_recursively_()
    {
        allocator().mark_(ptr_);
        ::gc::internal::trace_(*ptr_, [](auto ptr) {
            ptr.mark_recursively_();
        });
    }

};

template <typename T>
DEFINE_TRACEABLE(traced_ptr<T>)
{
    DEFINE_TRACE(const traced_ptr<T>& p)
    {
        tracer(p.ptr_);
    }
};

template <typename T, typename... Args>
traced_ptr<T> make_traced(Args&&... args)
{
    return traced_ptr<T>{std::forward<Args>(args)...};
}


} // end namespace gc
