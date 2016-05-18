#pragma once

#include "forward.h"

namespace gc
{

template<typename T>
class traced
{
    T      object_;
    bool   mark_;
    size_t ref_count_;
    size_t root_count_;

    template<typename... Args>
    traced(Args&& ... args)
            : object_(std::forward<Args>(args)...)
            , mark_{false}
            , ref_count_{0}
    { }

    template <typename S, typename Allocator, typename PAllocator>
    friend class traced_ptr;

    template <typename S, typename Allocator, typename PAllocator>
    friend class Collector;
};

} // end namespace gc
