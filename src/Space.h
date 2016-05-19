// Collector_base is the interface between the Collector<T>s and the
// CollectorManager.
#pragma once

namespace gc
{

class Collector;

namespace internal
{

class Space
{
    virtual void save_counts_()   =0;
    virtual void find_roots_()    =0;
    virtual void mark_()          =0;
    virtual void sweep_()         =0;

    friend class ::gc::Collector;

protected:
    virtual ~Space() { }
};

} // end namespace internal
} // end namespace gc