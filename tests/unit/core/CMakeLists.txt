# ==========================================
# tests/unit/core — nexus-core Unit Tests
# ==========================================
#
# Phase 7B first test: a smoke test verifying the GoogleTest + CTest
# + CMake testing infrastructure works end-to-end, using the existing
# nexus-core bootstrap stub from Phase 7A. No simulation engine logic
# is tested here.

nexus_add_test_executable(nexus-core-tests
    SOURCES
        bootstrap_test.cpp
    LIBS
        nexus::core
)