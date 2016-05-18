#pragma once

#include "forward.h"

namespace gc
{

template<typename T>
class Traced
{
    T    object_;
    bool mark_;

public:
    template<typename... Args>
    Traced(Args&& ... args)
            : object_{std::forward<Args>(args)...}
            , mark_{false}
    { }

    friend class traced_ptr<T>;
    friend class GC_allocator<T>;
};

} // end namespace gc
