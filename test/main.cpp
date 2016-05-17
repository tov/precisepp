#include <iostream>

#include "../src/gc.h"

using namespace std;

template<typename T>
gc::traced_ptr<T> t(T val)
{
    return gc::make_traced<T>(val);
}

template<typename T>
gc::traced_ptr<T> tr(const T& val)
{
    return gc::make_traced<T>(val);
}

int main()
{
    auto v = gc::make_traced<vector<gc::traced_ptr<int>>>(5, t(8));

    gc::internal::trace_(v, [](auto p){
        std::cout << "hi\n";
    });
}