#include "../src/gc.h"
#include "linked_list.h"

#include <iostream>

int main()
{
    auto c = cons(5, cons(6, cons(7, nullptr)));
    c->rest()->rest()->rest() = c;
    gc::Collector_manager::instance().collect();

}