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

void Collector::register_space_(Space& space)
{
    spaces_.push_back(&space);
}

void Collector::collect()
{
    using std::mem_fn;

    for_spaces(mem_fn(&Space::save_counts_));
    for_spaces(mem_fn(&Space::find_roots_));
    for_spaces(mem_fn(&Space::mark_));
    for_spaces(mem_fn(&Space::sweep_));
}

} // end namespace gc
