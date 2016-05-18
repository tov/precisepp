#include <iostream>
#include <memory>

#include "../src/gc.h"

using namespace std;

template<typename T>
using alloc = allocator<gc::Traced<T>>;

template<typename T>
using palloc = allocator<gc::Traced<T>*>;

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
    using intp = gc::traced_ptr<int>;
    using vecp = vector<intp>;
    auto v = gc::make_traced<vecp>(5, t(8));

    gc::Collector_manager::instance().collect();

    gc::internal::trace_(v, [](auto p){
        std::cout << "hi\n";
    });
}