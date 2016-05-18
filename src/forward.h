#pragma once

#include <memory>

namespace gc
{

template <typename T>
class Traced;

template <typename T,
          typename Allocator  = std::allocator<Traced<T>>,
          typename PAllocator = std::allocator<Traced<T>*>>
class traced_ptr;

template <typename T,
          typename Allocator  = std::allocator<Traced<T>>,
          typename PAllocator = std::allocator<Traced<T>*>>
class Collector;


} // end namespace gc