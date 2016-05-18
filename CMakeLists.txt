cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

set(GC_HEADERS
        src/util/count_map.h
        src/Allocator_manager.h
        src/forward.h
        src/gc.h
        src/GC_allocator.h
        src/stl.h
        src/Traceable.h
        src/Traced.h
        src/traced_ptr.h)

set(GC_LIB
        src/Allocator_manager.cpp
        ${GC_HEADERS})

set(SOURCE_FILES
        test/main.cpp
        ${GC_LIB})

add_executable(precisepp ${SOURCE_FILES})
set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)
