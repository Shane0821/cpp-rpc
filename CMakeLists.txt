PROJECT(rpc)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcoroutines)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/await)
endif()

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Options to build demo and test
option(BUILD_DEMO "Build the demo" OFF)
option(BUILD_TEST "Build the test" OFF)

add_subdirectory(src)

# Conditionally build demo
if (BUILD_DEMO)
    add_subdirectory(demo)
    add_custom_target(demo ALL
        DEPENDS librpc
    )
endif()

# Conditionally build test
if (BUILD_TEST)
    add_subdirectory(test)
    add_custom_target(test ALL
        DEPENDS librpc
    )
endif()