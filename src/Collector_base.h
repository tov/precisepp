#pragma once

namespace gc
{

class Collector_manager;

class Collector_base
{
    virtual void mark_()  =0;
    virtual void sweep_() =0;

    friend class Collector_manager;

protected:
    virtual ~Collector_base() { }
};

} // end namespace gc