nexus_add_test_executable(nexus-events-tests
    SOURCES
        test_event.cpp
        test_event_type.cpp
        test_event_queue.cpp
    LIBS
        nexus::core
)
