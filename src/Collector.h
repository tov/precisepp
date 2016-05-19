// The CollectorManager holds a pointer to each Collector<T>, and when a
// collection happens it has each TypedSpace<T> execute each phase.
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

    template <typename T, typename Allocator, typename PAllocator>
    friend class TypedSpace;
};

template <typename F>
void Collector::for_spaces(F f)
{
    for (internal::Space* space : spaces_)
        f(space);
}

} // end namespace gc