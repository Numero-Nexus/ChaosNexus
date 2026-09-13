add_executable(nexus-scheduler-tests
    test_fifo_policy.cpp
    test_priority_policy.cpp
    test_round_robin_policy.cpp
    test_scheduler.cpp
)

target_link_libraries(nexus-scheduler-tests
    PRIVATE
        nexus-core
        nexus-test-common
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(nexus-scheduler-tests)
