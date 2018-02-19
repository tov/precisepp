cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

include_directories(.)

set(GC_HEADERS
        precisepp/stl/array.h
        precisepp/stl/bitset.h
        precisepp/stl/deque.h
        precisepp/stl/forward_list.h
        precisepp/stl/list.h
        precisepp/stl/map.h
        precisepp/stl/memory.h
        precisepp/stl/set.h
        precisepp/stl/string.h
        precisepp/stl/tuple.h
        precisepp/stl/unordered_map.h
        precisepp/stl/unordered_set.h
        precisepp/stl/utility.h
        precisepp/stl/vector.h
        precisepp/Typed_space.h
        precisepp/Space.h
        precisepp/Collector.h
        precisepp/forward.h
        precisepp/gc.h
        precisepp/logger.h
        precisepp/stl.h
        precisepp/Traceable.h
        precisepp/Traced.h
        precisepp/traced_ptr.h)

set(GC_LIB
        precisepp/Collector.cpp
        precisepp/logging.cpp
        ${GC_HEADERS})

add_library(precisepp ${GC_LIB})

set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)

add_executable(precisepp-test test/main.cpp test/linked_list.h)
target_link_libraries(precisepp-test precisepp)

set_property(TARGET precisepp-test PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp-test PROPERTY CXX_STANDARD_REQUIRED On)

