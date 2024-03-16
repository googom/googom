cmake_minimum_required(VERSION 3.27)
project(Googom)

include(ExternalProject)  # Include the ExternalProject module

message(STATUS "CMake binary directory: ${CMAKE_BINARY_DIR}")

find_program(CCACHE_FOUND ccache)
if((CCACHE_FOUND))
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif ((CCACHE_FOUND))

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(Seastar_CXX_DIALECT "gnu++20" CACHE STRING "")

find_package (Seastar REQUIRED)

set(SOURCES
        main.cpp)

add_executable(Googom ${SOURCES})

target_link_libraries (Googom Seastar::seastar)