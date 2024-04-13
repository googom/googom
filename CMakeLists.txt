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

find_package(Boost REQUIRED COMPONENTS serialization)

set(SOURCES
        main.cpp
        topic_public/topic_public_definition.cpp
        topic_public/topic_public_definition.h
        topic_public/topic_public_structure.h
        topic_io/topic_io.cpp
        topic_io/topic_io.h
        topic_debugging/topic_debugging.cpp
        topic_debugging/topic_debugging.h
        utilities/print_utilities.cpp
        utilities/print_utilities.h
        topic_private/topic_offset/topic_private_offset_structure.h
        utilities/utils.cpp
        utilities/utils.h
        topic_public/topic_public_message.h)

add_executable(Googom ${SOURCES})

message(STATUS "Arrow version: ${ARROW_VERSION}")
message(STATUS "Arrow SO version: ${ARROW_FULL_SO_VERSION}")

include_directories(${Boost_INCLUDE_DIRS})

if(ARROW_LINK_SHARED)
    target_link_libraries(Googom PRIVATE Arrow::arrow_shared Seastar::seastar ${Boost_LIBRARIES})
else()
    target_link_libraries(Googom PRIVATE Arrow::arrow_static Seastar::seastar ${Boost_LIBRARIES})
endif()