// Traceable instances for STL containers and tuples

#pragma once

#include "tracing.h"

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <array>
#include <tuple>

namespace gc {

//
// Containers (except std::array)
//

TRACE_ITERABLE(std::deque);
TRACE_ITERABLE(std::forward_list);
TRACE_ITERABLE(std::list);
TRACE_ITERABLE(std::map);
TRACE_ITERABLE(std::queue);
TRACE_ITERABLE(std::set);
TRACE_ITERABLE(std::stack);
TRACE_ITERABLE(std::unordered_map);
TRACE_ITERABLE(std::unordered_set);
TRACE_ITERABLE(std::vector);

//
// std::array
//

template<typename E, size_t N>
struct Traceable<std::array<E, N>>
{
    template<typename F>
    static void trace(const std::array<E, N>& a, F tracer)
    {
        for (size_t i = 0; i < N; ++i)
            trace_<E, F>(a[i], tracer);
    }
};

//
// std::pair
//

template<typename K, typename V>
struct Traceable<std::pair<K, V>>
{
    template<typename F>
    static void trace(const std::pair<K, V>& p, F tracer)
    {
        trace_<K, F>(p.first, tracer);
        trace_<V, F>(p.second, tracer);
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

    using Ei   = std::tuple_element_t<i - 1, T>;
    using Next = TupleCountDown_<i - 1, T>;

    template<typename F>
    static void trace(const T& p, F tracer)
    {
        Next::template trace<F>(p, tracer);
        Traceable<Ei>::template trace<F>(std::get<i - 1>(p), tracer);
    }
};

template<typename T>
class TupleCountDown_<0, T>
{
    friend class TupleCountDown_<1, T>;

    template<typename F>
    static void trace(const T& p, F tracer) { }
};

} // end namespace internal

template<typename... E>
struct Traceable<std::tuple<E...>>
{
    using T     = std::tuple<E...>;
    using Start = internal::TupleCountDown_<std::tuple_size<T>::value, T>;

    template<typename F>
    static void trace(const T& p, F tracer)
    {
        Start::template trace<F>(p, tracer);
    }
};

} // end namespace gc
