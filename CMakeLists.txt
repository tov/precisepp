cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

include_directories(src)

set(GC_HEADERS
        src/Typed_space.h
        src/Space.h
        src/Collector.h
        src/forward.h
        src/gc.h
        src/logger.h
        src/stl.h
        src/Traceable.h
        src/Traced.h
        src/traced_ptr.h)

set(GC_LIB
        src/Collector.cpp
        src/logging.cpp
        ${GC_HEADERS})

add_library(precisepp ${GC_LIB})

set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)

add_executable(precisepp-test test/main.cpp test/linked_list.h)
target_link_libraries(precisepp-test precisepp)

set_property(TARGET precisepp-test PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp-test PROPERTY CXX_STANDARD_REQUIRED On)

