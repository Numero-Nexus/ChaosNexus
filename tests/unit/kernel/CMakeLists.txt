add_executable(nexus-kernel-tests
    test_component_registry.cpp
)

target_link_libraries(nexus-kernel-tests
    PRIVATE
        nexus-core
        nexus-test-common
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(nexus-kernel-tests)
