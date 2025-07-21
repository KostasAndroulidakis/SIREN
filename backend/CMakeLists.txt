cmake_minimum_required(VERSION 3.16)
project(SIREN_backend VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard Configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific flags for military-grade performance
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Modern Boost find package (CMP0167 policy)
cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Threads REQUIRED)

# OpenSSL for military-grade TLS 1.3 encryption
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
    message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found - required for military-grade security")
endif()

# Placeholder for source files (will be added incrementally)
set(ALL_SOURCES
    # Files will be added as they are created
)

# Create library target first (will build incrementally)
add_library(SIREN_lib INTERFACE)
target_include_directories(SIREN_lib INTERFACE include)

# Link libraries to interface library
target_link_libraries(SIREN_lib
    INTERFACE
    Boost::system
    Boost::thread
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Platform-specific configurations
if(APPLE)
    # macOS specific optimizations for M1
    target_compile_definitions(SIREN_lib INTERFACE BOOST_ASIO_HAS_KQUEUE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        target_compile_options(SIREN_lib INTERFACE -mcpu=apple-m1)
    endif()
endif()

# Debug information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")

# Only create executable if main.cpp exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
    # Collect existing source files
    file(GLOB_RECURSE EXISTING_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
    )

    # Ensure statistics_calculator.cpp is included
    if(NOT "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/statistics_calculator.cpp" IN_LIST EXISTING_SOURCES)
        list(APPEND EXISTING_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/statistics_calculator.cpp")
    endif()

    if(EXISTING_SOURCES)
        add_executable(SIREN_server ${EXISTING_SOURCES})
        target_link_libraries(SIREN_server PRIVATE SIREN_lib)
        install(TARGETS SIREN_server RUNTIME DESTINATION bin)
        message(STATUS "Created executable with sources: ${EXISTING_SOURCES}")
    endif()
else()
    message(STATUS "src/main.cpp not found - skipping executable creation")
endif()

# Testing Support (Optional)
option(BUILD_TESTS "Build hardware-in-the-loop tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()