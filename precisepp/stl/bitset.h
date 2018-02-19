#pragma once

#include "../Traceable.h"
#include <bitset>

template <size_t size>
DEFINE_TRACEABLE_UNTRACED_REF_T(std::bitset<size>);

