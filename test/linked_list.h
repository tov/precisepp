#pragma once

#include "../src/gc.h"
#include <utility>

template <typename T>
class node
{
public:
    using link_t = gc::traced_ptr<node<T>>;

    node(T first, link_t rest)
            : first_{first}, rest_{rest}
    { }

    const T& first() const { return first_; }
    T& first() { return first_; }

    const link_t& rest() const { return rest_; }
    link_t& rest() { return rest_; }

private:
    T first_;
    link_t rest_;

    friend class gc::Traceable<node<T>>;
};

template <typename T>
DEFINE_TRACEABLE(node<T>) {
    DEFINE_TRACE(const node<T>& n)
    {
        TRACE(n.first_);
        TRACE(n.rest_);
    }
};

template <typename T>
using list = typename node<T>::link_t;

template <typename T>
list<T> cons(const T& first, list<T> rest)
{
    return gc::make_traced<node<T>>(first, rest);
}

template <typename T>
list<T> cons(T&& first, list<T> rest)
{
    return gc::make_traced<node<T>>(std::move(first), rest);
}


