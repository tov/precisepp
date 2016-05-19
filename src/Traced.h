#pragma once

#include <cassert>
#include "forward.h"

namespace gc
{

template<typename T>
class traced
{
    union
    {
        struct
        {
            traced<T>* next_free;
        } free;

        struct
        {
            T      object;
            size_t ref_count;
            size_t root_count;
        } used;

        struct {
            size_t     page_size;
            traced<T>* next_page;
        } header;
    }      union_;
    bool   mark_;
    bool   free_;

    traced<T>*& next_free_()    { return union_.free.next_free; }
    T& object_()                { return union_.used.object; }
    size_t& ref_count_()        { return union_.used.ref_count; }
    size_t& root_count_()       { return union_.used.root_count; }
    size_t& page_size_()        { return union_.header.page_size; }
    traced<T>*& next_page_()    { return union_.header.next_page; }

    void initialize_free_(traced<T>* next_free = nullptr)
    {
        next_free_() = next_free;
        free_ = true;
    }

    template <typename... Args>
    void initialize_used_(Args&& ... args)
    {
        ::new(&object_()) T(std::forward<Args>(args)...);
        ref_count_() = 0;
        mark_ = false;
        free_ = false;
    }

    void initialize_header_(size_t page_size, traced<T>* next_page)
    {
        page_size_() = page_size;
        next_page_() = next_page;
    }

    template <typename S, typename Allocator, typename PAllocator>
    friend class traced_ptr;

    template <typename S, typename Allocator, typename PAllocator>
    friend class TypedSpace;
};

} // end namespace gc
