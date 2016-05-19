#include "Collector.h"

namespace gc
{

Collector::Collector() = default;

Collector& Collector::instance()
{
    static Collector manager;
    return manager;
}

void Collector::register_space_(internal::Space& collector)
{
    collectors_.push_back(&collector);
}

void Collector::collect()
{
    typedef void(internal::Space::*action_t)();

    for_collectors([](auto space) { space->save_counts_(); });
    for_collectors([](auto space) { space->find_roots_(); });
    for_collectors([](auto space) { space->mark_(); });
    for_collectors([](auto space) { space->sweep_(); });
}

} // end namespace gc
