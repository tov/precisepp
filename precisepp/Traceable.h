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

#define CONTAINS_POINTERS_IF(...) \
    static constexpr bool contains_pointers_v = __VA_ARGS__

namespace internal
{

template <typename... Es>
struct contains_pointers;

template <typename E, typename... Es>
struct contains_pointers<E, Es...>
{
    static constexpr bool value = ::gc::Traceable<E>::contains_pointers
                                  || contains_pointers<Es...>::value;
};

template <>
struct contains_pointers<>
{
    static constexpr bool value = false;
};

} // end namespace internal

template <typename... Es>
constexpr bool contains_pointers = internal::contains_pointers<Es...>::value;

#define DEFINE_TRACEABLE(...) \
    class ::gc::Traceable<__VA_ARGS__>

#define TO_TRACE(...) \
    template <typename S__, typename F__>\
    friend void ::gc::internal::trace(S__&&, F__);\
    template <typename tracer_t>\
    static void trace(__VA_ARGS__, tracer_t tracer)

#define TRACE(...) \
    ::gc::internal::trace(__VA_ARGS__, tracer)

#define DEFINE_TRACEABLE_UNTRACED_VALUE_T(...) \
    DEFINE_TRACEABLE(__VA_ARGS__) \
    {\
        CONTAINS_POINTERS_IF(false);\
        TO_TRACE(__VA_ARGS__) { }\
    }

#define DEFINE_TRACEABLE_UNTRACED_VALUE(...) \
    template <>\
    DEFINE_TRACEABLE_UNTRACED_VALUE_T(__VA_ARGS__);

#define DEFINE_TRACEABLE_UNTRACED_REF_T(...) \
    DEFINE_TRACEABLE(__VA_ARGS__) \
    {\
        CONTAINS_POINTERS_IF(false);\
        TO_TRACE(const __VA_ARGS__&) { }\
    }

#define DEFINE_TRACEABLE_UNTRACED_REF(...) \
    template <>\
    DEFINE_TRACEABLE_UNTRACED_REF_T(__VA_ARGS__);

DEFINE_TRACEABLE_UNTRACED_VALUE(bool);
DEFINE_TRACEABLE_UNTRACED_VALUE(unsigned char);
DEFINE_TRACEABLE_UNTRACED_VALUE(signed char);
DEFINE_TRACEABLE_UNTRACED_VALUE(char);
DEFINE_TRACEABLE_UNTRACED_VALUE(wchar_t);
DEFINE_TRACEABLE_UNTRACED_VALUE(char16_t);
DEFINE_TRACEABLE_UNTRACED_VALUE(char32_t);
DEFINE_TRACEABLE_UNTRACED_VALUE(short);
DEFINE_TRACEABLE_UNTRACED_VALUE(unsigned short);
DEFINE_TRACEABLE_UNTRACED_VALUE(int);
DEFINE_TRACEABLE_UNTRACED_VALUE(unsigned int);
DEFINE_TRACEABLE_UNTRACED_VALUE(long);
DEFINE_TRACEABLE_UNTRACED_VALUE(unsigned long);
DEFINE_TRACEABLE_UNTRACED_VALUE(long long);
DEFINE_TRACEABLE_UNTRACED_VALUE(unsigned long long);
DEFINE_TRACEABLE_UNTRACED_VALUE(float);
DEFINE_TRACEABLE_UNTRACED_VALUE(double);
DEFINE_TRACEABLE_UNTRACED_VALUE(long double);

#define DEFINE_TRACEABLE_CONTAINER(C) \
    template <typename E, typename... Rest> \
    DEFINE_TRACEABLE(C<E, Rest...>)\
    {\
        CONTAINS_POINTERS_IF(::gc::contains_pointers<E>);\
        TO_TRACE(const C<E, Rest...>& v)\
        {\
            if (Traceable::contains_pointers_v)\
                for (const auto& e : v)\
                     TRACE(e);\
        }\
    }

#define DEFINE_TRACEABLE_CONTAINER2(C) \
    template <typename E1, typename E2, typename... Rest> \
    DEFINE_TRACEABLE(C<E1, E2, Rest...>)\
    {\
        CONTAINS_POINTERS_IF(::gc::contains_pointers<E1, E2>);\
        TO_TRACE(const C<E1, E2, Rest...>& v)\
        {\
            if (Traceable::contains_pointers_v)\
                for (const auto& e : v)\
                     TRACE(e);\
        }\
    }

} // end namespace gc

