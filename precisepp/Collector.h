// The `Collector` holds a pointer to each `Typed_space<T>` (via the
// interface `Space`). When a collection happens it has each space execute
// each phase in turn.
#pragma once

#include "Space.h"
#include "forward.h"

#include <vector>

namespace gc
{

class Collector
{
public:
    static Collector& instance();

    void collect();

private:
    std::vector<detail::Space*> spaces_;

    Collector();

    void register_space(detail::Space&);

    template <typename F>
    void for_spaces_(F);

    template <typename T, typename Allocator>
    friend class Typed_space;
};

template <typename F>
void Collector::for_spaces_(F f)
{
    for (detail::Space* space : spaces_)
        f(space);
}

} // end namespace gc