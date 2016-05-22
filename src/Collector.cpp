#include "Collector.h"

#include "logger.h"

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

    log(debug2) << "collect: save_counts";
    for_spaces_(mem_fn(&Space::save_counts));
    log(debug2) << "collect: find_roots";
    for_spaces_(mem_fn(&Space::find_roots));
    log(debug2) << "collect: mark";
    for_spaces_(mem_fn(&Space::mark));
    log(debug2) << "collect: sweep";
    for_spaces_(mem_fn(&Space::sweep));
    log(debug2) << "collect: done";
}

} // end namespace gc
