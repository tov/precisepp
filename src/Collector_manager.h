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

    void mark_();
    void sweep_();

    template <typename T>
    friend class Collector;
};

} // end namespace gc