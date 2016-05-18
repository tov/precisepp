// Forward declarations of templates

#pragma once

#include <memory>

namespace gc
{

template <typename T>
class traced;

template <typename T,
          typename Allocator  = std::allocator<traced<T>>,
          typename PAllocator = std::allocator<traced<T>*>>
class traced_ptr;

template <typename T,
          typename Allocator  = std::allocator<traced<T>>,
          typename PAllocator = std::allocator<traced<T>*>>
class Collector;


} // end namespace gc