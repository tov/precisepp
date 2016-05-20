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
void trace(T&& object, F tracer)
{
    Traceable<std::remove_cv_t<std::remove_reference_t<T>>>
        ::template trace<F>(std::forward<T>(object), tracer);
};

} // end namespace internal

#define DEFINE_TRACE(...) \
    template <typename S__, typename F__>\
    friend void ::gc::internal::trace(S__&&, F__);\
    template <typename tracer_t>\
    static void trace(__VA_ARGS__, tracer_t tracer)

#define DEFINE_TRACEABLE(...) \
    class ::gc::Traceable<__VA_ARGS__>

#define TRACE(...) \
    ::gc::internal::trace(__VA_ARGS__, tracer)

#define DEFINE_TRACEABLE_UNTRACED(...) \
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
                ::gc::internal::trace(e, tracer);\
        }\
    }

DEFINE_TRACEABLE_UNTRACED(bool);
DEFINE_TRACEABLE_UNTRACED(unsigned char);
DEFINE_TRACEABLE_UNTRACED(signed char);
DEFINE_TRACEABLE_UNTRACED(char);
DEFINE_TRACEABLE_UNTRACED(wchar_t);
DEFINE_TRACEABLE_UNTRACED(char16_t);
DEFINE_TRACEABLE_UNTRACED(char32_t);
DEFINE_TRACEABLE_UNTRACED(short);
DEFINE_TRACEABLE_UNTRACED(unsigned short);
DEFINE_TRACEABLE_UNTRACED(int);
DEFINE_TRACEABLE_UNTRACED(unsigned int);
DEFINE_TRACEABLE_UNTRACED(long);
DEFINE_TRACEABLE_UNTRACED(unsigned long);
DEFINE_TRACEABLE_UNTRACED(long long);
DEFINE_TRACEABLE_UNTRACED(unsigned long long);
DEFINE_TRACEABLE_UNTRACED(float);
DEFINE_TRACEABLE_UNTRACED(double);
DEFINE_TRACEABLE_UNTRACED(long double);

} // end namespace gc

