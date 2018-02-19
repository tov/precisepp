#pragma once

#include "../Traceable.h"
#include <memory>

template<typename T, typename Deleter>
DEFINE_TRACEABLE(std::unique_ptr<T, Deleter>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<T>);
    TO_TRACE(const std::unique_ptr<T, Deleter>& p)
    {
        TRACE(*p);
    }
};

template<typename T>
DEFINE_TRACEABLE(std::shared_ptr<T>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<T>);
    TO_TRACE(const std::shared_ptr<T>& p)
    {
        TRACE(*p);
    }
};

template<typename T>
DEFINE_TRACEABLE(std::weak_ptr<T>)
{
    CONTAINS_POINTERS_IF(::gc::contains_pointers<T>);
    TO_TRACE(const std::weak_ptr<T>& p)
    {
        if (!p.expired()) {
            TRACE(*p.lock());
        }
    }
};

