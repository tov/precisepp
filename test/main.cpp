#include "../src/gc.h"
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

int main()
{
    collect();
    auto c = cons(5, cons(6, cons(7, nullptr)));
    collect();
    c->rest()->rest()->rest() = c;
    collect();

    c = nullptr;
    collect();
}