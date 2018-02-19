# Precise++: A precise garbage collector for C++

This library provides a new kind of smart pointer, `gc::traced_ptr<T>` which 
enables memory management by a tracing garbage collector. This does not add 
tracing gc to existing programs, just as adding `std::shared_ptr<T>` to the 
library did not enable reference counting in existing programs. However, it 
enables garbage collection for new linked data structures that are designed 
to use it.

For example, suppose we want garbage collected linked lists. We can define a 
linked list node struct with support for tracing by using `gc::traced_ptr` as
our pointer to the next node:

```cpp
template <typename T>
struct node
{
    using link_t = gc::traced_ptr<node>;
    
    node(const T& f, const link_t& r) : first(f), rest(r) { }
    
    T first;
    link_t rest;
};
```

Then we tell the garbage collector how to trace a node:

```cpp
template <typename T>
DEFINE_TRACEABLE(node<T>) {
    CONTAINS_POINTERS_IF(true);
    TO_TRACE(const node<T>& n)
    {
        TRACE(n.first);
        TRACE(n.rest);
    }
};
```

Then we can program with garbage-collected linked lists, like so:

```cpp
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
```

Unlike with `std::shared_ptr`, we can safely make cycles and they will be 
collected.

## Why not use this thing?

It's untested. No one has ever built a significant thing using it.

It's undocumented and probably hard to understand how to use correctly.

It's probably really slow. It's definitely slower than `std::shared_ptr`, 
because it includes reference counting as part of its root tracking.

It has some other limitations. Currently it uses the program's control stack 
for recursive tracing, which means that on long lists it blows the stack.

