#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <tuple>

namespace gc
{

template <typename T>
struct Traceable;

#define TRACE_NO_PTRS(T) \
    template <>\
    struct Traceable<T>\
    {\
        template <typename F>\
        static void trace(T, F) {}\
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

#define TRACE_ITERABLE(C) \
    template <typename... E> \
    struct Traceable<C<E...>>\
    {\
        template <typename F>\
        static void trace(const C<E...>& v, F tracer)\
        {\
            for (const auto& e : v)\
                Traceable<typename C<E...>::value_type>::template trace<F>(e, tracer);\
        }\
    }

template <typename K, typename V>
struct Traceable<std::pair<K, V>>
{
    template <typename F>
    static void trace(const std::pair<K, V>& p, F tracer)
    {
        Traceable<K>::template trace<F>(p.first, tracer);
        Traceable<V>::template trace<F>(p.second, tracer);
    }
};

TRACE_ITERABLE(std::vector);
TRACE_ITERABLE(std::set);
TRACE_ITERABLE(std::unordered_set);
TRACE_ITERABLE(std::map);
TRACE_ITERABLE(std::unordered_map);

template <size_t i, typename T>
struct CountDown_
{
    using Ei = typename std::tuple_element<i - 1, T>::type;

    template <typename F>
    static void trace(const T& p, F tracer)
    {
        Traceable<Ei>::template trace<F>(std::get<i - 1>(p), tracer);
        CountDown_<i - 1, T>::template trace<F>(p, tracer);
    }
};

template <typename T>
struct CountDown_<0, T>
{
    template <typename F>
    static void trace(const T& p, F tracer) { }
};

template <typename... E>
struct Traceable<std::tuple<E...>>
{
    using T = std::tuple<E...>;

    template <typename F>
    static void trace(const T& p, F tracer)
    {
        CountDown_<std::tuple_size<T>::value, T>::template trace<F>(p, tracer);
    }
};


template <typename T, typename = Traceable<T>>
struct traced_ptr
{
    struct impl
    {
        template <typename... Args>
        impl(Args&&... args)
                : data_{std::forward<Args>(args)...}
                , mark_{false}
        { std::make_shared<int>(4); }

        T            data_;
        mutable bool mark_;
    };

    impl* pimpl_ = nullptr;
};

template <typename T>
struct Traceable<traced_ptr<T>>
{
    template <typename F>
    static void trace(const traced_ptr<T>& p, F tracer)
    {
        tracer(p);
    }
};

template <typename T, typename F>
void trace(T&& object, F tracer)
{
    Traceable<std::remove_reference_t<T>>::
        template trace<F>(std::forward<T>(object), tracer);
};

template <typename T, typename... Args>
traced_ptr<T> make_traced(Args&&... args)
{
    traced_ptr<T> result;
    result.pimpl_ = new typename traced_ptr<T>::impl(std::forward<Args>(args)...);
    return result;
}

} // end namespace gc