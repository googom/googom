cmake_minimum_required(VERSION 3.27)
project(Googom)

include(ExternalProject)  # Include the ExternalProject module

option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)

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

find_package(Arrow REQUIRED)

set(SOURCES
        main.cpp
        topic_public/topic_definition.cpp
        topic_public/topic_definition.h
        topic_public/topic_structure.h
        topic_io/topic_io.cpp
        topic_io/topic_io.h
        topic_debugging/topic_debugging.cpp
        topic_debugging/topic_debugging.h)

add_executable(Googom ${SOURCES})

message(STATUS "Arrow version: ${ARROW_VERSION}")
message(STATUS "Arrow SO version: ${ARROW_FULL_SO_VERSION}")


if(ARROW_LINK_SHARED)
    target_link_libraries(Googom PRIVATE Arrow::arrow_shared Seastar::seastar)
else()
    target_link_libraries(Googom PRIVATE Arrow::arrow_static Seastar::seastar)
endif()