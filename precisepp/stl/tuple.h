#pragma once

#include "../Traceable.h"
#include <tuple>

namespace gc {
namespace internal {

template<size_t i, typename T>
class TupleCountDown_
{
    friend class Traceable<T>;

    friend class TupleCountDown_<i + 1, T>;

    using Next = TupleCountDown_<i - 1, T>;

    TO_TRACE(const T& p)
    {
        // This isn't infinite recursion because of the specialization below.
        Next::template trace<tracer_t>(p, tracer);
        TRACE(std::get<i - 1>(p));
    }
};

template<typename T>
class TupleCountDown_<0, T>
{
    friend class TupleCountDown_<1, T>;

    TO_TRACE(const T&) {}
}

} // end namespace internal
} // end namespace gc

template<typename... E>
DEFINE_TRACEABLE(std::tuple<E...>)
{
    using T     = std::tuple<E...>;
    using Start = internal::TupleCountDown_<std::tuple_size<T>::value, T>;

    CONTAINS_POINTERS_IF(::gc::contains_pointers<E...>);

    TO_TRACE(const T& p)
    {
        Start::template trace<tracer_t>(p, tracer);
    }
};

