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
    std::vector<internal::Space*> collectors_;

    Collector();

    void register_space_(internal::Space&);

    void save_counts_();
    void find_roots_();
    void mark_();
    void sweep_();

    template <typename T, typename Allocator, typename PAllocator>
    friend class TypedSpace;
};

} // end namespace gc