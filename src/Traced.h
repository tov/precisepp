#pragma once

#include "forward.h"

namespace gc
{

template<typename T>
class Traced
{
    T    object_;
    bool mark_;
    int  refcount_;

    template<typename... Args>
    Traced(Args&& ... args)
            : object_(std::forward<Args>(args)...)
            , mark_{false}
            , refcount_{0}
    { }

    friend class traced_ptr<T>;

    template <typename S>
    friend class Collector;
};

} // end namespace gc
