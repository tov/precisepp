cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

set(GC_HEADERS
        src/util/count_map.h
        src/Collector.h
        src/Collector_base.h
        src/Collector_manager.h
        src/forward.h
        src/gc.h
        src/stl.h
        src/Traceable.h
        src/Traced.h
        src/traced_ptr.h)

set(GC_LIB
        src/Collector_manager.cpp
        ${GC_HEADERS})

set(SOURCE_FILES
        test/main.cpp
        ${GC_LIB})

add_executable(precisepp ${SOURCE_FILES})
set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)
