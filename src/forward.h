// Forward declarations of templates

#pragma once

#include <memory>

namespace gc
{

template <typename T>
class Traced;

template <typename T,
          typename Allocator = std::allocator<Traced<T>>>
class traced_ptr;

template <typename T,
          typename Allocator = std::allocator<Traced<T>>>
class Typed_space;


} // end namespace gc