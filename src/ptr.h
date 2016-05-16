#pragma once

#include "tracing.h"
#include "stl.h"

#include <utility>
#include <unordered_set>

namespace gc
{

template <typename T>
class Allocator;

template <typename T, typename = Traceable<T>>
class traced_ptr
{
    friend class Allocator<T>;
    friend class Traceable<traced_ptr<T>>;

    struct impl
    {
        template <typename... Args>
        impl(Args&&... args)
                : data_{std::forward<Args>(args)...}
                , refcount_{1}
                , mark_{false}
        { }

        T              data_;
        mutable size_t refcount_;
        mutable bool   mark_;
    };

    impl* pimpl_;

    void inc_()
    {
        if (pimpl_ != nullptr) ++pimpl_->refcount_;
    }

    void dec_()
    {
        if (pimpl_ != nullptr) --pimpl_->refcount_;
    }

public:
    traced_ptr() : pimpl_{nullptr}
    { }

    traced_ptr(impl* pimpl) : pimpl_{pimpl}
    {
        inc_();
    }

    traced_ptr(const traced_ptr& other)
    {
        pimpl_ = other.pimpl_;
        inc_();
    }

    traced_ptr(traced_ptr&& other)
    {
        std::swap(pimpl_, other.pimpl_);
    }

    traced_ptr& operator=(const traced_ptr& other)
    {
        dec_();
        pimpl_ = other.pimpl_;
        inc_();
        return *this;
    }

    traced_ptr& operator=(traced_ptr&& other)
    {
        std::swap(pimpl_, other.pimpl_);
        return *this;
    }

    ~traced_ptr()
    {
        dec_();
    }

};

template <typename T>
DEFINE_TRACEABLE(traced_ptr<T>)
{
    DEFINE_TRACE(const traced_ptr<T>& p)
    {
        tracer(p.pimpl_);
    }
};

template <typename T, typename... Args>
traced_ptr<T> make_traced(Args&&... args)
{
    return Allocator<T>::instance()
            .template make_traced<Args...>(std::forward<Args>(args)...);
}

template <typename T>
class Allocator
{
public:
    static Allocator& instance() noexcept
    {
        static Allocator instance_;
        return instance_;
    }

    template <typename... Args>
    traced_ptr<T> make_traced(Args&&... args)
    {
        traced_ptr<T> result;
        result.pimpl_ = new typename traced_ptr<T>::impl(std::forward<Args>(args)...);
        objects_.insert(result.pimpl_);
        return result;
    }

private:
    Allocator()
    { }

    std::unordered_set<typename traced_ptr<T>::impl*> objects_;
};

} // end namespace gc
