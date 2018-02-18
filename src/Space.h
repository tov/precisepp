// Space is the interface between the TypedSpace<T>s and the
// Collector. Each Typed_space<T> manages only objects of type T,
// but to collect properly we need to mark the whole pointer graph
// (unless we can prove disjointness!). Thus, we provide this
// interface to the Collector, and it then makes the spaces cooperate.

#pragma once

#include <cstddef>

namespace gc
{

class Collector;

namespace internal
{

class Space
{
    // The client of this interface is the `Collector`.
    friend class ::gc::Collector;


    // Our garbage collection proceeds in four phases, which must be
    // run for each space in turn; that is, every space must run phase 1,
    // then every space must run phase 2, etc. Here are the phases:

    // Phase 1: Copies every `Traced<T>`’s `ref_count_` to `root_count_`.
    virtual void save_counts()    =0;

    // Phase 2: Decrements `root_count_` for every in-edge coming from
    // another traced object. When finished, roots will have non-zero
    // `root_count_`.
    virtual void find_roots()     =0;

    // Phase 3: Marks the live heap starting with the roots found in the
    // previous phase.
    virtual void mark()           =0;

    // Phase 4: Sweeps away the dead heap, deallocating and resetting marks.
    virtual void sweep()          =0;


    // Stats, currently unused.

    // The size of `T` for each `Space<T>`.
    virtual size_t element_size() const =0;

    // The number of objects this `Space` can hold without growing.
    virtual size_t total_slots() const     =0;

    // Over-approximation of the number of live objects this `Space` currently
    // holds.
    virtual size_t used_slots() const         =0;

protected:
    virtual ~Space() = default;
};

} // end namespace internal
} // end namespace gc