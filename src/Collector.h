// The CollectorManager holds a pointer to each Collector<T>, and when a
// collection happens it has each Typed_space<T> execute each phase.
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
    std::vector<internal::Space*> spaces_;

    Collector();

    void register_space_(internal::Space&);

    template <typename F>
    void for_spaces(F);

    template <typename T, typename Allocator>
    friend class Typed_space;
};

template <typename F>
void Collector::for_spaces(F f)
{
    for (internal::Space* space : spaces_)
        f(space);
}

} // end namespace gc