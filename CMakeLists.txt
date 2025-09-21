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

include(FetchContent)

# Options to build demo and test
option(BUILD_DEMO "Build the demo" OFF)
option(BUILD_TEST "Build the test" OFF)

# ============================
# Directories
# ============================

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${ROOT_DIR}/src")
set(INCLUDE_DIR "${ROOT_DIR}/include")
set(DEMO_DIR "${ROOT_DIR}/demo")
set(TEST_DIR "${ROOT_DIR}/test")
set(THIRD_PARTY_DIR "${ROOT_DIR}/3rd")
set(PB_DIR "${ROOT_DIR}/proto/stub")
set(CONFIG_DIR "${ROOT_DIR}/config")

# ============================
# Includes
# ============================

include_directories(
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}/protobuf/src
    ${THIRD_PARTY_DIR}/llbc/llbc/include
)

# ============================
# Libraries
# ============================

link_directories(
    ${THIRD_PARTY_DIR}/protobuf/src/.libs
    ${THIRD_PARTY_DIR}/llbc/output/gmake/release64
)

# ============================
# Pakcages
# ============================

find_library(libzk zookeeper_mt)

# ============================
# fmt
# ============================

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

# ============================
# Files
# ============================

file(GLOB_RECURSE CONFIG_FILES ${CONFIG_DIR}/*)
file(COPY ${CONFIG_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)

# ============================
# Build
# ============================

add_subdirectory(src)

# Conditionally build demo
if (BUILD_DEMO)
    add_subdirectory(demo)
    add_custom_target(demo ALL
        DEPENDS rpc
    )
endif()

# Conditionally build test
if (BUILD_TEST)
    add_subdirectory(test)
    add_custom_target(test ALL
        DEPENDS rpc
    )
endif()