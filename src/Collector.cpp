#include "Collector.h"

#include <functional>

namespace gc
{

Collector::Collector() = default;

Collector& Collector::instance()
{
    static Collector manager;
    return manager;
}

void Collector::register_space_(internal::Space& space)
{
    spaces_.push_back(&space);
}

void Collector::collect()
{
    using std::mem_fn;

    for_spaces(mem_fn(&internal::Space::save_counts_));
    for_spaces(mem_fn(&internal::Space::find_roots_));
    for_spaces(mem_fn(&internal::Space::mark_));
    for_spaces(mem_fn(&internal::Space::sweep_));
}

} // end namespace gc
