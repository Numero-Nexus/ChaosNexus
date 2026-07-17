# ==========================================
# tests/unit/text — String Utilities Unit Tests
# ==========================================
#
# Phase 8F: String Utilities test suite.

nexus_add_test_executable(nexus-text-tests
    SOURCES
        test_strings.cpp
    LIBS
        nexus::core
)
