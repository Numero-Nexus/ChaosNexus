# ==========================================
# nexus-test-common — Shared Test Utilities
# ==========================================
#
# A small static library of test-only helpers shared across unit,
# integration, and performance test suites. Kept distinct from
# tests/fixtures/, which holds reusable GoogleTest fixture classes
# and scenario data rather than free-standing utility functions.

add_library(nexus-test-common STATIC
    src/bootstrap.cpp
)

target_include_directories(nexus-test-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(nexus-test-common PUBLIC cxx_std_23)