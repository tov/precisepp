// A traced_ptr<T> is a garbage-collected pointer to a T.

#pragma once

#include "forward.h"
#include "Traceable.h"
#include "Traced.h"

#include <utility>
#include <unordered_set>
#include <cassert>
#include <memory>
#include <vector>

namespace gc
{

template <typename T, typename Allocator>
class traced_ptr
{
public:
    using element_type = T;
    using pointer      = T*;

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

    operator bool()
    {
        return ptr_ != nullptr;
    }

    pointer get() const
    {
        return &ptr_->object_();
    }

    element_type& operator*() const
    {
        return ptr_->object_();
    }

    pointer operator->() const
    {
        return get();
    }

    void swap(traced_ptr& other)
    {
        std::swap(ptr_, other.ptr_);
    }

private:
    friend class Traceable<traced_ptr>;
    friend class Typed_space<T, Allocator>;

    Traced<T>* ptr_;

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

template <typename T, typename Allocator>
DEFINE_TRACEABLE(traced_ptr<T, Allocator>)
{
    DEFINE_TRACE(const traced_ptr<T, Allocator>& p)
    {
        tracer(p.ptr_);
    }
};

template <typename T, typename Allocator>
void swap(traced_ptr<T, Allocator>& a, traced_ptr<T, Allocator>& b)
{
    a.swap(b);
};

template <typename T1, typename Allocator1,
          typename T2, typename Allocator2>
bool operator==(const traced_ptr<T1, Allocator1>& a,
                const traced_ptr<T2, Allocator2>& b)
{
    return a.get() == b.get();
};

template <typename T1, typename Allocator1,
          typename T2, typename Allocator2>
bool operator!=(const traced_ptr<T1, Allocator1>& a,
                const traced_ptr<T2, Allocator2>& b)
{
    return a.get() != b.get();
};

template <typename T1, typename Allocator1,
          typename T2, typename Allocator2>
bool operator<(const traced_ptr<T1, Allocator1>& a,
               const traced_ptr<T2, Allocator2>& b)
{
    // From unique_ptr:
    using CT = typename
      std::common_type<typename traced_ptr<T1, Allocator1>::pointer,
                       typename traced_ptr<T2, Allocator2>::pointer>::type;
    return std::less<CT>(a.get(), b.get());
};

template <typename T1, typename Allocator1,
        typename T2, typename Allocator2>
bool operator<=(const traced_ptr<T1, Allocator1>& a,
                const traced_ptr<T2, Allocator2>& b)
{
    return !(b < a);
};

template <typename T1, typename Allocator1,
        typename T2, typename Allocator2>
bool operator>(const traced_ptr<T1, Allocator1>& a,
               const traced_ptr<T2, Allocator2>& b)
{
    return b < a;
};

template <typename T1, typename Allocator1,
        typename T2, typename Allocator2>
bool operator>=(const traced_ptr<T1, Allocator1>& a,
                const traced_ptr<T2, Allocator2>& b)
{
    return !(a < b);
};

template <typename T, typename Allocator>
bool operator==(std::nullptr_t, const traced_ptr<T, Allocator>& b)
{
    return nullptr == b.get();
};

template <typename T, typename Allocator>
bool operator==(const traced_ptr<T, Allocator>& a, std::nullptr_t)
{
    return a.get() == nullptr;
};

template <typename T, typename Allocator>
bool operator!=(std::nullptr_t, const traced_ptr<T, Allocator>& b)
{
    return nullptr != b.get();
};

template <typename T, typename Allocator>
bool operator!=(const traced_ptr<T, Allocator>& a, std::nullptr_t)
{
    return a.get() != nullptr;
};

template <typename T, typename Allocator>
bool operator<(std::nullptr_t, const traced_ptr<T, Allocator>& b)
{
    using param = typename traced_ptr<T, Allocator>::pointer;
    return std::less<param>()(nullptr, b.get());
};

template <typename T, typename Allocator>
bool operator<(const traced_ptr<T, Allocator>& a, std::nullptr_t)
{
    using param = typename traced_ptr<T, Allocator>::pointer;
    return std::less<param>()(a.get(), nullptr);
};

template <typename T, typename Allocator>
bool operator<=(std::nullptr_t, const traced_ptr<T, Allocator>& b)
{
    return !(b < nullptr);
};

template <typename T, typename Allocator>
bool operator<=(const traced_ptr<T, Allocator>& a, std::nullptr_t)
{
    return !(nullptr < a);
};

template <typename T, typename Allocator>
bool operator>(std::nullptr_t, const traced_ptr<T, Allocator>& b)
{
    return b < nullptr;
};

template <typename T, typename Allocator>
bool operator>(const traced_ptr<T, Allocator>& a, std::nullptr_t)
{
    return nullptr < a;
};

template <typename T, typename Allocator>
bool operator>=(std::nullptr_t, const traced_ptr<T, Allocator>& b)
{
    return !(nullptr < b);
};

template <typename T, typename Allocator>
bool operator>=(const traced_ptr<T, Allocator>& a, std::nullptr_t)
{
    return !(a < nullptr);
};

} // end namespace gc
