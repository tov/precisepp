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


int main()
{
    list<int> c = nullptr;

    for (int i = 0; i < 165508; ++i) {
        std::cerr << i << std::endl;
        c = cons((int) i, c);
    }

    c = cons(0, c);

    // collect();
}