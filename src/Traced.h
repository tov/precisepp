#pragma once

#include "forward.h"

namespace gc
{

template<typename T>
class Traced
{
    T    object_;
    bool mark_;

    template<typename... Args>
    Traced(Args&& ... args)
            : object_{std::forward<Args>(args)...}
            , mark_{false}
    { }

    friend class traced_ptr<T>;

    template <typename S>
    friend class Collector;
};

} // end namespace gc
