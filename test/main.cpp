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

gc::traced_ptr<int> i(int n)
{
    return t(n);
}

int main()
{
    auto w{ t(std::make_tuple(4, t(5), t(vector<gc::traced_ptr<int>>{t(1)}))) };

//    gc::internal::trace_(v, [](auto p){
//        std::cout << "hi\n";
//    });

    return 0;
}