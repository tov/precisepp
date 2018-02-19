#pragma once

#include "../Traceable.h"
#include <array>

template<typename E, size_t N>
DEFINE_TRACEABLE(std::array<E, N>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<E>);
    TO_TRACE(const std::array<E, N>& a)
    {
        if (Traceable::contains_pointers_v)
            for (size_t i = 0; i < N; ++i)
                TRACE(a[i]);
    }
};

