// Traceable instances for STL containers and tuples

#pragma once

#include "Traceable.h"

#include <bitset>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <memory>

#include <array>
#include <tuple>

namespace gc {

//
// Some STL objects that don't contain pointers
//

DEFINE_TRACEABLE_UNTRACED_REF(std::string);

template <size_t size>
DEFINE_TRACEABLE_UNTRACED_REF_T(std::bitset<size>);

//
// Smart pointers
//

template<typename T, typename Deleter>
DEFINE_TRACEABLE(std::unique_ptr<T, Deleter>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<T>);
    TO_TRACE(const std::unique_ptr<T, Deleter>& p)
    {
        TRACE(*p);
    }
};

template<typename T>
DEFINE_TRACEABLE(std::shared_ptr<T>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<T>);
    TO_TRACE(const std::shared_ptr<T>& p)
    {
        TRACE(*p);
    }
};

template<typename T>
DEFINE_TRACEABLE(std::weak_ptr<T>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<T>);
    TO_TRACE(const std::weak_ptr<T>& p)
    {
        if (!p.expired()) {
            TRACE(*p.lock());
        }
    }
};

//
// Containers (except std::array)
//

DEFINE_TRACEABLE_CONTAINER(std::deque);
DEFINE_TRACEABLE_CONTAINER(std::forward_list);
DEFINE_TRACEABLE_CONTAINER(std::list);
DEFINE_TRACEABLE_CONTAINER2(std::map);
DEFINE_TRACEABLE_CONTAINER(std::set);
DEFINE_TRACEABLE_CONTAINER2(std::unordered_map);
DEFINE_TRACEABLE_CONTAINER(std::unordered_set);
DEFINE_TRACEABLE_CONTAINER(std::vector);

//
// std::array
//

template<typename E, size_t N>
DEFINE_TRACEABLE(std::array<E, N>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<E>);
    TO_TRACE(const std::array<E, N>& a)
    {
        if (Traceable::contains_pointers_v)
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
    CONTAINS_POINTERS_IF(::gc::contains_pointers<K, V>);
    TO_TRACE(const std::pair<K, V>& p)
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

    TO_TRACE(const T&p)
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
    TO_TRACE(const T&) { }
};

} // end namespace internal

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

} // end namespace gc
