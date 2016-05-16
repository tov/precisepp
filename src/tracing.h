// For tracing pointers. The main idea is that we specialize the
// template Traceable<T> for each type that can be traced, and it
// provides a tracing function that traverses the type.

#pragma once

#include <type_traits>
#include <utility>

namespace gc
{

template<typename T>
class Traceable;

template<typename T, typename F>
void trace_(T&& object, F tracer)
{
    Traceable<std::remove_reference_t<T>>::
    template trace<F>(std::forward<T>(object), tracer);
};

#define TRACE_NO_PTRS(T) \
    template <>\
    struct ::gc::Traceable<T>\
    {\
        template <typename F>\
        static void trace(T, F) {}\
    }

#define TRACE_ITERABLE(C) \
    template <typename... E> \
    struct ::gc::Traceable<C<E...>>\
    {\
        template <typename F>\
        static void trace(const C<E...>& v, F tracer)\
        {\
            for (const auto& e : v)\
                Traceable<typename C<E...>::value_type>::template trace<F>(e, tracer);\
        }\
    }

TRACE_NO_PTRS(bool);
TRACE_NO_PTRS(unsigned char);
TRACE_NO_PTRS(signed char);
TRACE_NO_PTRS(short);
TRACE_NO_PTRS(unsigned short);
TRACE_NO_PTRS(int);
TRACE_NO_PTRS(unsigned int);
TRACE_NO_PTRS(long);
TRACE_NO_PTRS(unsigned long);
TRACE_NO_PTRS(float);
TRACE_NO_PTRS(double);

} // end namespace gc

