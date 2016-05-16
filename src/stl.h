// Traceable instances for STL containers and tuples

#pragma once

#include "tracing.h"

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <array>
#include <tuple>

namespace gc {

//
// Containers (except std::array)
//

DEFINE_TRACEABLE_CONTAINER(std::deque);
DEFINE_TRACEABLE_CONTAINER(std::forward_list);
DEFINE_TRACEABLE_CONTAINER(std::list);
DEFINE_TRACEABLE_CONTAINER(std::map);
DEFINE_TRACEABLE_CONTAINER(std::set);
DEFINE_TRACEABLE_CONTAINER(std::unordered_map);
DEFINE_TRACEABLE_CONTAINER(std::unordered_set);
DEFINE_TRACEABLE_CONTAINER(std::vector);

//
// std::array
//

template<typename E, size_t N>
DEFINE_TRACEABLE(std::array<E, N>)
{
    DEFINE_TRACE(const std::array<E, N>& a)
    {
        for (size_t i = 0; i < N; ++i)
            TRACE(a[i]);
    }
};

//
// std::pair
//

template<typename K, typename V>
DEFINE_TRACEABLE(std::pair<K, V>)
{
    DEFINE_TRACE(const std::pair<K, V>& p)
    {
        TRACE(p.first);
        TRACE(p.second);
    }
};

//
// std::tuple
//

namespace internal {

template<size_t i, typename T>
class TupleCountDown_
{
    friend class Traceable<T>;
    friend class TupleCountDown_<i + 1, T>;

    using Next = TupleCountDown_<i - 1, T>;

    DEFINE_TRACE(const T&p)
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
    DEFINE_TRACE(const T&) { }
};

} // end namespace internal

template<typename... E>
DEFINE_TRACEABLE(std::tuple<E...>)
{
    using T     = std::tuple<E...>;
    using Start = internal::TupleCountDown_<std::tuple_size<T>::value, T>;

    DEFINE_TRACE(const T& p)
    {
        Start::template trace<tracer_t>(p, tracer);
    }
};

} // end namespace gc
