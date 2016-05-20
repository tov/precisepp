#include "Collector.h"

#include <functional>

namespace gc
{

using namespace internal;

Collector::Collector() = default;

Collector& Collector::instance()
{
    static Collector manager;
    return manager;
}

void Collector::register_space(Space& space)
{
    spaces_.push_back(&space);
}

void Collector::collect()
{
    using std::mem_fn;

    for_spaces_(mem_fn(&Space::save_counts));
    for_spaces_(mem_fn(&Space::find_roots));
    for_spaces_(mem_fn(&Space::mark));
    for_spaces_(mem_fn(&Space::sweep));
}

} // end namespace gc
