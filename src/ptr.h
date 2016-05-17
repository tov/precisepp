#pragma once

#include "tracing.h"
#include "stl.h"
#include "util/count_map.h"

#include <utility>
#include <unordered_set>
#include <cassert>
#include <memory>

namespace gc
{

template <typename T>
class Allocator;

namespace internal
{

template <typename T>
struct traced_box_
{
    template <typename... Args>
    traced_box_(Args&&... args)
            : data_{std::forward<Args>(args)...}
            , refcount_{0}
            , mark_{false}
    {
        ::gc::internal::trace_(data_, [](auto ptr) {
            ptr.dec_();
        });
    }

    ~traced_box_()
    {
        ::gc::internal::trace_(data_, [](auto ptr) {
            ptr.inc_();
        });
    }

    T              data_;
    mutable size_t refcount_;
    mutable bool   mark_;
};

} // end namespace internal

template <typename T, typename = Traceable<T>>
class traced_ptr
{
    friend class Allocator<T>;
    friend class Traceable<traced_ptr<T>>;

    template<typename S>
    friend class internal::traced_box_;

    internal::traced_box_<T>* pimpl_;

    void inc_()
    {
        if (pimpl_ != nullptr) ++pimpl_->refcount_;
    }

    void dec_()
    {
        if (pimpl_ != nullptr) {
            assert(pimpl_->refcount_ > 0);
            --pimpl_->refcount_;
        }
    }

    void clear_()
    {
        if (pimpl_ != nullptr) pimpl_->mark_ = false;
    }

    void mark_recursively_()
    {
        if (pimpl_ != nullptr && !pimpl_->mark_) {
            pimpl_->mark_ = true;
            ::gc::internal::trace_(pimpl_->data_, [](auto ptr) {
                ptr->mark_recursively_();
            });
        }
    }

public:
    traced_ptr() : pimpl_{nullptr}
    { }

    traced_ptr(internal::traced_box_<T>* pimpl) : pimpl_{pimpl}
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
        tracer(p);
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
    using impl = internal::traced_box_<T>;
public:
    static Allocator& instance() noexcept
    {
        static Allocator instance_;
        return instance_;
    }

    template <typename... Args>
    traced_ptr<T> make_traced(Args&&... args)
    {
        auto pimpl = new impl{std::forward<Args>(args)...};
        traced_ptr<T> result{pimpl};

        objects_.insert(pimpl);

        return result;
    }

private:
    Allocator()
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


    count_map<internal::traced_box_<T>*> roots_;
    std::unordered_set<internal::traced_box_<T>*> objects_;
};

} // end namespace gc
