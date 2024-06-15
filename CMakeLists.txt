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

# Use static Boost libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Find required packages
find_package(Seastar REQUIRED)
find_package(Arrow REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization unit_test_framework program_options thread chrono atomic)

# Ensure Boost libraries are static
foreach(boost_lib ${Boost_LIBRARIES})
    if(boost_lib MATCHES ".+\\.so")
        message(FATAL_ERROR "Dynamic Boost library found: ${boost_lib}. Ensure all Boost libraries are static (.a)")
    endif()
endforeach()

# Include directories
include_directories(${Boost_INCLUDE_DIRS})

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
        managers/distributed_topic_manager.cpp
        managers/distributed_topic_manager.h
)

# Add the executable target
add_executable(Googom ${SOURCES})

# Output Arrow version information
message(STATUS "Arrow version: ${ARROW_VERSION}")
message(STATUS "Arrow SO version: ${ARROW_FULL_SO_VERSION}")

# Link libraries statically if possible, else fallback to shared
set(SSL_LIB "")
set(CRYPTO_LIB "")

# Check for static versions of SSL and Crypto libraries
find_library(SSL_STATIC_LIBRARY NAMES ssl libssl.a PATHS /usr/lib/x86_64-linux-gnu /usr/local/lib)
find_library(CRYPTO_STATIC_LIBRARY NAMES crypto libcrypto.a PATHS /usr/lib/x86_64-linux-gnu /usr/local/lib)

if(SSL_STATIC_LIBRARY)
    set(SSL_LIB ${SSL_STATIC_LIBRARY})
else()
    find_library(SSL_SHARED_LIBRARY NAMES ssl libssl.so PATHS /usr/lib/x86_64-linux-gnu /usr/local/lib)
    set(SSL_LIB ${SSL_SHARED_LIBRARY})
endif()

if(CRYPTO_STATIC_LIBRARY)
    set(CRYPTO_LIB ${CRYPTO_STATIC_LIBRARY})
else()
    find_library(CRYPTO_SHARED_LIBRARY NAMES crypto libcrypto.so PATHS /usr/lib/x86_64-linux-gnu /usr/local/lib)
    set(CRYPTO_LIB ${CRYPTO_SHARED_LIBRARY})
endif()

# Force static linking for Boost and Seastar
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

# Link Arrow statically if possible
if(NOT ARROW_LINK_SHARED AND TARGET Arrow::arrow_static)
    set(ARROW_LIB Arrow::arrow_static)
else()
    set(ARROW_LIB Arrow::arrow)
endif()

# Link Seastar statically if possible
if(TARGET Seastar::seastar_static)
    set(SEASTAR_LIB Seastar::seastar_static)
else()
    set(SEASTAR_LIB Seastar::seastar)
endif()

# Link the executable with the determined libraries
target_link_libraries(Googom PRIVATE ${ARROW_LIB} ${SEASTAR_LIB} ${Boost_LIBRARIES} ${SSL_LIB} ${CRYPTO_LIB} -latomic -ldl -lpthread -lrt)

# Force static linking for Boost
foreach(boost_lib ${Boost_LIBRARIES})
    if(boost_lib MATCHES ".+\\.so")
        message(FATAL_ERROR "Dynamic Boost library found: ${boost_lib}. Ensure all Boost libraries are static (.a)")
    endif()
endforeach()
