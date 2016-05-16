#include <iostream>

#include "../src/gc.h"

using namespace std;

gc::traced_ptr<int> i(int n)
{
    return gc::make_traced<int>(n);
}

int main()
{
    auto v{ std::make_tuple(4, i(5), vector<gc::traced_ptr<int>>{i(1), i(2), i(3) }) };

    gc::internal::trace_(v, [](auto p){
        std::cout << "hi\n";
    });

    return 0;
}