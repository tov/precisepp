#pragma once

#include "tracing.h"
#include "stl.h"

#include <utility>

namespace gc
{

template <typename T, typename = Traceable<T>>
class traced_ptr
{
public:
    template <typename... Args>
    static traced_ptr<T> make_traced(Args&&... args)
    {
        traced_ptr<T> result;
        result.pimpl_ = new typename traced_ptr<T>::impl(std::forward<Args>(args)...);
        return result;
    }

private:
    struct impl
    {
        template <typename... Args>
        impl(Args&&... args)
                : data_{std::forward<Args>(args)...}
                , mark_{false}
        { }

        T            data_;
        mutable bool mark_;
    };

    impl* pimpl_ = nullptr;
};

template <typename T>
struct Traceable<traced_ptr<T>>
{
    template <typename F>
    static void trace(const traced_ptr<T>& p, F tracer)
    {
        tracer(p);
    }
};

template <typename T, typename... Args>
traced_ptr<T> make_traced(Args&&... args)
{
    return traced_ptr<T>::template make_traced<Args...>(std::forward<Args>(args)...);
}

} // end namespace gc