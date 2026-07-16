nexus_add_test_executable(nexus-identity-tests
    SOURCES
        test_uuid.cpp
        test_strong_id.cpp
    LIBS
        nexus::core
)
