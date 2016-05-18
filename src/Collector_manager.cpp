#include "Collector_manager.h"

namespace gc
{

Collector_manager::Collector_manager() = default;

Collector_manager& Collector_manager::instance()
{
    static Collector_manager manager;
    return manager;
}

void Collector_manager::register_collector_(Collector_base& collector)
{
    collectors_.push_back(&collector);
}

void Collector_manager::mark_()
{
    for (Collector_base* collector : collectors_)
        collector->sweep_();
}

void Collector_manager::sweep_()
{
    for (Collector_base* collector : collectors_)
        collector->sweep_();
}

void Collector_manager::collect()
{
    mark_();
    sweep_();
}

} // end namespace gc
