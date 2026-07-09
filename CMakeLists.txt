# ==========================================
# ChaosNexus — Root CMakeLists.txt
# ==========================================
#
# Phase 7A: Repository Bootstrap & Development Environment.
# This file wires together the module skeletons created in Step 2.
# No simulation engine logic is configured here — only enough build
# structure to prove the toolchain works end-to-end.

cmake_minimum_required(VERSION 3.28)

project(ChaosNexus
    VERSION 0.1.0
    DESCRIPTION "Deterministic distributed systems simulation engine"
    LANGUAGES CXX
)

# ------------------------------------------
# Global language configuration
# ------------------------------------------
# CMAKE_CXX_EXTENSIONS OFF forces strict ISO C++23 (-std=c++23) rather
# than compiler-specific extensions (-std=gnu++23). This keeps the
# codebase portable and avoids relying on non-standard behavior.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------
# Out-of-source build guard
# ------------------------------------------
# Prevents accidental in-source builds, which pollute the repository
# and defeat the purpose of a clean, out-of-source CMake workflow.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed. Please use an out-of-source "
        "build, e.g.:\n"
        "  cmake --preset default\n"
        "  cmake --build --preset default"
    )
endif()

# ------------------------------------------
# Project-wide options
# ------------------------------------------
option(NEXUS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(NEXUS_ENABLE_SANITIZERS  "Enable AddressSanitizer/UBSan instrumentation" OFF)
option(NEXUS_BUILD_TESTS "Build the ChaosNexus test suite" ON)

# ------------------------------------------
# Shared CMake modules
# ------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

# ------------------------------------------
# Modules (dependency order: core -> sdk -> consumers -> plugins)
# ------------------------------------------
add_subdirectory(nexus-core)
add_subdirectory(nexus-sdk)
add_subdirectory(nexus-cli)
add_subdirectory(nexus-bench)
add_subdirectory(nexus-lab)
add_subdirectory(nexus-plugins)

# ------------------------------------------
# Tests
# ------------------------------------------
if(NEXUS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ------------------------------------------
# Apply shared warning policy to buildable targets
# ------------------------------------------
# nexus-sdk is INTERFACE-only and nexus-plugins declares no targets
# yet, so only the compiled targets receive the warning policy here.
nexus_set_warnings(nexus-core)
nexus_set_warnings(nexus-cli)
nexus_set_warnings(nexus-bench)
nexus_set_warnings(nexus-lab)

nexus_enable_sanitizers(nexus-core)
nexus_enable_sanitizers(nexus-cli)
nexus_enable_sanitizers(nexus-bench)
nexus_enable_sanitizers(nexus-lab)

# ------------------------------------------
# Build summary
# ------------------------------------------
message(STATUS "ChaosNexus ${PROJECT_VERSION} configuration summary:")
message(STATUS "  C++ standard:          C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler:              ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Warnings as errors:    ${NEXUS_WARNINGS_AS_ERRORS}")
message(STATUS "  Sanitizers enabled:    ${NEXUS_ENABLE_SANITIZERS}")