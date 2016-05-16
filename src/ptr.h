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
private:
    friend class Allocator<T>;
    friend class Traceable<traced_ptr<T>>;

    struct impl
    {
        template <typename... Args>
        impl(Args&&... args)
                : data_{std::forward<Args>(args)...}
                , refcount_{0}
                , mark_{false}
        { }

        T              data_;
        mutable size_t refcount_;
        mutable bool   mark_;
    };

    impl* pimpl_ = nullptr;
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
    std::unordered_set<typename traced_ptr<T>::impl*> roots_;
};

} // end namespace gc
