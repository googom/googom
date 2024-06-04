cmake_minimum_required(VERSION 3.27)
project(Googom)

# Copy configs directory to the binary directory
file(COPY ${CMAKE_SOURCE_DIR}/configs DESTINATION ${CMAKE_BINARY_DIR})

include(ExternalProject)  # Include the ExternalProject module

option(ARROW_LINK_SHARED "Link to the Arrow shared library" OFF)

message(STATUS "CMake binary directory: ${CMAKE_BINARY_DIR}")

# Use ccache if found
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Seastar C++ dialect
set(Seastar_CXX_DIALECT "gnu++20" CACHE STRING "")

# Find required packages
find_package(Seastar REQUIRED)
find_package(Arrow REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)

# Source files
set(SOURCES
        main.cpp
        topics/topic_public/topic_public_definition.cpp
        topics/topic_public/topic_public_definition.h
        topics/topic_public/topic_public_structure.h
        topics/topic_io/topic_io.cpp
        topics/topic_io/topic_io.h
        topics/topic_debugging/topic_debugging.cpp
        topics/topic_debugging/topic_debugging.h
        utilities/print_utilities.cpp
        utilities/print_utilities.h
        topics/topic_private/topic_offset/topic_private_offset_structure.h
        utilities/utils.cpp
        utilities/utils.h
        topics/topic_public/topic_public_message.h
        topics/topic_private/topic_offset/topic_private_offset_definition.cpp
        topics/topic_private/topic_offset/topic_private_offset_definition.h
        communication/http/rest_server.cpp
        communication/http/rest_server.h
        communication/temp_store/message_store.cpp
        communication/temp_store/message_store.h
        managers/topic_manager.cpp
        managers/topic_manager.cpp
        managers/topic_manager.h
        managers/config_manager.cpp
        managers/config_manager.h
        managers/initialization_manager.cpp
        managers/initialization_manager.h
        topics/topic_private/topic_user/topic_private_user_structure.h
        topics/topic_private/topic_user/topic_private_user_definition.cpp
        topics/topic_private/topic_user/topic_private_user_definition.h
        communication/tcp/tcp_server.cpp
        communication/tcp/tcp_server.h
        communication/tcp/tcp_session.h
        communication/communication_utils/communication_utils.h
)

# Add the executable target
add_executable(Googom ${SOURCES})

# Output Arrow version information
message(STATUS "Arrow version: ${ARROW_VERSION}")
message(STATUS "Arrow SO version: ${ARROW_FULL_SO_VERSION}")

# Include directories
include_directories(${Boost_INCLUDE_DIRS})

# Link libraries statically
target_link_libraries(Googom PRIVATE Arrow::arrow_static Seastar::seastar ${Boost_LIBRARIES})
