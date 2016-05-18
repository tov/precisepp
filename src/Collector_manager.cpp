#include "Collector_manager.h"

namespace gc
{

Collector_manager::Collector_manager() = default;

Collector_manager& Collector_manager::instance()
{
    static Collector_manager manager;
    return manager;
}

void Collector_manager::register_collector_(internal::Collector_base& collector)
{
    collectors_.push_back(&collector);
}

void Collector_manager::save_counts_()
{
    for (auto collector : collectors_)
        collector->save_counts_();
}

void Collector_manager::find_roots_()
{
    for (auto collector : collectors_)
        collector->find_roots_();
}

void Collector_manager::mark_()
{
    for (auto collector : collectors_)
        collector->mark_();
}

void Collector_manager::sweep_()
{
    for (auto collector : collectors_)
        collector->sweep_();
}

void Collector_manager::collect()
{
    save_counts_();
    find_roots_();
    mark_();
    sweep_();
}

} // end namespace gc
