cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

set(GC_HEADERS
        src/gc.h
        src/traced_ptr.h
        src/Traceable.h
        src/stl.h
        src/GC_allocator.h
        src/Allocator_manager.h
        src/Allocator_manager.cpp
        src/util/count_map.h)

set(GC_LIB
        src/gc.cpp
        ${GC_HEADERS})

set(SOURCE_FILES
        test/main.cpp
        ${GC_LIB})

add_executable(precisepp ${SOURCE_FILES})
set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)
