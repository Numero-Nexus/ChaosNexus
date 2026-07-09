# ==========================================
# ChaosNexus — Test Suite Root
# ==========================================
#
# Phase 7B: Testing Infrastructure.
#
# Integrates GoogleTest via FetchContent (pinned release tag, built
# from source using the same compiler/CRT settings as the rest of
# the project — see 05_IMPLEMENTATION_GUIDE.md / Phase 7B Step 2 for
# the full rationale on why FetchContent was chosen over CPM, vcpkg,
# or ExternalProject at this stage).

include(FetchContent)

# gtest_force_shared_crt: matches ChaosNexus's own dynamic CRT default
# on MSVC. Without this, GoogleTest defaults to static CRT linkage,
# producing LNK2038 "RuntimeLibrary mismatch" errors when linked
# against targets built with the dynamic CRT.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(googletest)

# ------------------------------------------
# Shared test-target helper
# ------------------------------------------
# Centralizes the boilerplate every test executable needs: linking
# against GoogleTest, applying the project's warning policy, and
# registering discovered tests with CTest. This mirrors the
# nexus_set_warnings() pattern established in Phase 7A.
include(GoogleTest)

function(nexus_add_test_executable target)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})

    add_executable(${target} ${ARG_SOURCES})

    target_link_libraries(${target}
        PRIVATE
            GTest::gtest
            GTest::gtest_main
            nexus-test-common
            ${ARG_LIBS}
    )

    target_compile_features(${target} PRIVATE cxx_std_23)
    nexus_set_warnings(${target})

    gtest_discover_tests(${target})
endfunction()

# ------------------------------------------
# Test subdirectories
# ------------------------------------------
add_subdirectory(common)
add_subdirectory(fixtures)
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(performance)
add_subdirectory(benchmarks)