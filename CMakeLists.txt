cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

set(GC_HEADERS
        src/TypedSpace.h
        src/Space.h
        src/Collector.h
        src/forward.h
        src/gc.h
        src/stl.h
        src/Traceable.h
        src/traced.h
        src/traced_ptr.h)

set(GC_LIB
        src/Collector.cpp
        ${GC_HEADERS})

set(SOURCE_FILES
        test/main.cpp
        test/linked_list.h
        ${GC_LIB})

add_executable(precisepp ${SOURCE_FILES})
set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)
