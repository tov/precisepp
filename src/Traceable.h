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

namespace internal
{

template<typename T, typename F>
void trace_(T&& object, F tracer)
{
    Traceable<std::remove_cv_t<std::remove_reference_t<T>>>
        ::template trace<F>(std::forward<T>(object), tracer);
};

} // end namespace internal

#define DEFINE_TRACE(...) \
    template <typename S__, typename F__>\
    friend void ::gc::internal::trace_(S__&&, F__);\
    template <typename tracer_t>\
    static void trace(__VA_ARGS__, tracer_t tracer)

#define DEFINE_TRACEABLE(...) \
    class ::gc::Traceable<__VA_ARGS__>

#define TRACE(...) \
    ::gc::internal::trace_(__VA_ARGS__, tracer)

#define DEFINE_TRACEABLE_POD(...) \
    template <>\
    DEFINE_TRACEABLE(__VA_ARGS__) \
    {\
        DEFINE_TRACE(__VA_ARGS__) {}\
    }

#define DEFINE_TRACEABLE_CONTAINER(C) \
    template <typename... E> \
    DEFINE_TRACEABLE(C<E...>)\
    {\
        DEFINE_TRACE(const C<E...>& v)\
        {\
            for (const auto& e : v) \
                ::gc::internal::trace_(e, tracer);\
        }\
    }

DEFINE_TRACEABLE_POD(bool);
DEFINE_TRACEABLE_POD(unsigned char);
DEFINE_TRACEABLE_POD(signed char);
DEFINE_TRACEABLE_POD(char);
DEFINE_TRACEABLE_POD(wchar_t);
DEFINE_TRACEABLE_POD(char16_t);
DEFINE_TRACEABLE_POD(char32_t);
DEFINE_TRACEABLE_POD(short);
DEFINE_TRACEABLE_POD(unsigned short);
DEFINE_TRACEABLE_POD(int);
DEFINE_TRACEABLE_POD(unsigned int);
DEFINE_TRACEABLE_POD(long);
DEFINE_TRACEABLE_POD(unsigned long);
DEFINE_TRACEABLE_POD(long long);
DEFINE_TRACEABLE_POD(unsigned long long);
DEFINE_TRACEABLE_POD(float);
DEFINE_TRACEABLE_POD(double);
DEFINE_TRACEABLE_POD(long double);

} // end namespace gc

