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

void Collector::save_counts_()
{
    for (auto collector : collectors_)
        collector->save_counts_();
}

void Collector::find_roots_()
{
    for (auto collector : collectors_)
        collector->find_roots_();
}

void Collector::mark_()
{
    for (auto collector : collectors_)
        collector->mark_();
}

void Collector::sweep_()
{
    for (auto collector : collectors_)
        collector->sweep_();
}

void Collector::collect()
{
    save_counts_();
    find_roots_();
    mark_();
    sweep_();
}

} // end namespace gc
