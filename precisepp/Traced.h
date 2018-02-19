// The per-object metadata for each object of type `T` is stored, with that
// object, in a `Traced<T>`. The `Traced<T>`s are allocated in arrays that
// are managed by `Typed_space<T>`s.
#pragma once

#include <cassert>
#include "forward.h"

namespace gc
{

template<typename T>
class Traced
{
    // Each `Traced<T>` is in one of three states:
    //
    //   - The first slot in every page is a *header*, which contains the
    //     number of slots in the page (itself included) and a link to the
    //     next page.
    //
    //   - The *free* state, indicated by the `free_` bit being set, means
    //     that there is no object here, and the `Traced<T>` is a member of the
    //     free list.
    //
    //   - Otherwise it’s in the *used* state, meaning it contains a
    //     (potentially) live object.
    //
    // The three members of the union represent the data held in each of the
    // three states. See below for a member function for initializing each
    // state.
    union
    {
        struct {
            size_t     page_size;
            Traced<T>* next_page;
        } header;

        struct
        {
            Traced<T>* next_free;
        } free;

        struct
        {
            T      object;
            size_t ref_count;
            size_t root_count;
        } used;
    }      union_;

    // The free bit, set when this `Traced<T>` is on the free list.
    bool   free_;

    // The mark bit, used during the marking and sweeping phases of collection.
    bool   mark_;

    //
    // Accessor functions to avoid having to write `ptr->union_.header.stuff`
    // all over the place.
    //

    size_t& page_size_()        { return union_.header.page_size; }
    Traced<T>*& next_page_()    { return union_.header.next_page; }

    Traced<T>*& next_free_()    { return union_.free.next_free; }

    T& object_()                { return union_.used.object; }
    size_t& ref_count_()        { return union_.used.ref_count; }
    size_t& root_count_()       { return union_.used.root_count; }

    //
    // Initialization functions
    //

    // Initializes a `Traced<T>` as a page header, give its size (header
    // included) and a pointer to the next page in the page list.
    void initialize_header_(size_t page_size, Traced<T>* next_page)
    {
        page_size_() = page_size;
        next_page_() = next_page;
    }

    // Initializes a `Traced<T>` to the free state, adding it to the given free
    // list.
    void initialize_free_(Traced<T>* next_free = nullptr)
    {
        next_free_() = next_free;
        free_ = true;
    }

    // Initializes a `Traced<T>` to the used state (except for initializing
    // the `T` object itself).
    void initialize_used_()
    {
        ref_count_() = 0;
        mark_ = false;
        free_ = false;
    }

    template <typename S, typename Allocator>
    friend class traced_ptr;

    template <typename S, typename Allocator>
    friend class Typed_space;
};

} // end namespace gc
