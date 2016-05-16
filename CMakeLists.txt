cmake_minimum_required(VERSION 3.3)
project(precisepp CXX)

set(SOURCE_FILES
        src/main.cpp
        src/gc.cpp
        src/gc.h)

add_executable(precisepp ${SOURCE_FILES})
set_property(TARGET precisepp PROPERTY CXX_STANDARD 14)
set_property(TARGET precisepp PROPERTY CXX_STANDARD_REQUIRED On)
