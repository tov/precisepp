#pragma once

#include "precisepp/gc.h"
#include <utility>

template <typename T>
struct node
{
public:
    using link_t = gc::traced_ptr<node>;

    node(T f, link_t r) : first{f}, rest{r} { }

    T first;
    link_t rest;
};

template <typename T>
DEFINE_TRACEABLE(node<T>) {
    CONTAINS_POINTERS_IF(true);
    TO_TRACE(const node<T>& n)
    {
        TRACE(n.first);
        TRACE(n.rest);
    }
};

template <typename T>
using list = typename node<T>::link_t;

template <typename T>
list<T> cons(T first, list<T> rest)
{
    return gc::make_traced<node<T>>(first, rest);
}

template<typename T>
list<T> append(list<T> before, list<T> after)
{
    if (!before) return after;

    auto new_node = cons(before->first, nullptr);
    auto result = new_node;
    before = before->rest;

    while (before) {
        new_node->rest = cons(before->first, nullptr);
        new_node = new_node->rest;
        before = before->rest;
    }

    new_node->rest = after;

    return result;
}

template<typename T>
void concat(list<T>& before, list<T> after)
{
    list<T>* place = &before;
    while (*place) place = &(*place)->rest;
    *place = after;
}
