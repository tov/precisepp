#include "../src/gc.h"
#include "linked_list.h"

#include <iostream>

using namespace std;

template<typename T>
using alloc = allocator<gc::traced<T>>;

template<typename T>
using palloc = allocator<gc::traced<T>*>;

template<typename T>
gc::traced_ptr<T> t(T val)
{
    return gc::make_traced<T, alloc<T>, palloc<T>>(val);
}

template<typename T>
gc::traced_ptr<T> tr(const T& val)
{
    return gc::make_traced<T, alloc<T>, palloc<T>>(val);
}

int main()
{
    auto c = cons(5, cons(6, cons(7, nullptr)));
    gc::Collector_manager::instance().collect();

}