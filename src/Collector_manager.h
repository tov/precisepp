// The CollectorManager holds a pointer to each Collector<T>, and when a
// collection happens it has each Collector<T> execute each phase.
#pragma once

#include "Collector_base.h"
#include "forward.h"

#include <vector>

namespace gc
{

class Collector_manager
{
public:
    static Collector_manager& instance();

    void collect();

private:
    std::vector<Collector_base*> collectors_;

    Collector_manager();

    void register_collector_(Collector_base&);

    void save_counts_();
    void find_roots_();
    void mark_();
    void sweep_();

    template <typename T, typename Allocator, typename PAllocator>
    friend class Collector;
};

} // end namespace gc