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
        public_topic/topic_definition.cpp
        public_topic/topic_definition.h
        public_topic/topic_structure.h)

add_executable(Googom ${SOURCES})

message(STATUS "Arrow version: ${ARROW_VERSION}")
message(STATUS "Arrow SO version: ${ARROW_FULL_SO_VERSION}")


if(ARROW_LINK_SHARED)
    target_link_libraries(Googom PRIVATE Arrow::arrow_shared Seastar::seastar)
else()
    target_link_libraries(Googom PRIVATE Arrow::arrow_static Seastar::seastar)
endif()