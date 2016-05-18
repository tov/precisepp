#pragma once

#include "forward.h"

namespace gc
{

template<typename T>
class Traced
{
    T      object_;
    bool   mark_;
    size_t ref_count_;
    size_t root_count_;

    template<typename... Args>
    Traced(Args&& ... args)
            : object_(std::forward<Args>(args)...)
            , mark_{false}
            , ref_count_{0}
    { }

    friend class traced_ptr<T>;

    template <typename S>
    friend class Collector;
};

} // end namespace gc
