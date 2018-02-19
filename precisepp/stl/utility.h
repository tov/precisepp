#pragma once

#include "../Traceable.h"
#include <utility>

template<typename K, typename V>
DEFINE_TRACEABLE(std::pair<K, V>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<K, V>);
    TO_TRACE(const std::pair<K, V>& p)
    {
        TRACE(p.first);
        TRACE(p.second);
    }
};

