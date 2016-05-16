cmake_minimum_required(VERSION 3.5)
project(pgccpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
        src/main.cpp
        src/gc.cpp
        src/gc.h)
add_executable(pgccpp ${SOURCE_FILES})