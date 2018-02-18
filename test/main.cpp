#include "gc.h"
#include "linked_list.h"

#include <iostream>

void collect()
{
    auto& space = gc::Typed_space<node<int>>::instance();
    std::cerr << "before: H = " << space.total_slots();
    std::cerr << ", L = " << space.used_slots() << '\n';

    gc::Collector::instance().collect();

    std::cerr << "after:  H = " << space.total_slots();
    std::cerr << ", L = " << space.used_slots() << "\n\n";
}

list<int> make_list(int size) {
    list<int> result;
    while (size--) {
        result = cons(size, result);
    }
    return result;
}

list<int> make_loop(int size) {
    list<int> result = make_list(size);
    concat<int>(result, result);
    return result;
}

int main()
{
    collect();

    for (int i = 0; i < 10; ++i) {
        make_loop(20'000);
    }

    collect();
}