#pragma once

#include "forward.h"

namespace gc
{

template<typename T>
class Traced
{
    T    object_;
    bool mark_;
#ifdef DEBUG_REFCOUNTS
    int  refcount_;
#endif

    template<typename... Args>
    Traced(Args&& ... args)
            : object_(std::forward<Args>(args)...)
            , mark_{false}
#ifdef DEBUG_REFCOUNTS
            , refcount_{0}
#endif
    { }

    friend class traced_ptr<T>;

    template <typename S>
    friend class Collector;
};

} // end namespace gc
