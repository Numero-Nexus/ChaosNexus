# ==========================================
# nexus-bench — Benchmarking Framework
# ==========================================
#
# This is a bootstrap placeholder for Phase 7A (Repository Bootstrap).
# No benchmarking logic exists yet. This target exists only to verify
# the executable builds and links against nexus-core and nexus-sdk.

add_executable(nexus-bench
    src/main.cpp
)

target_link_libraries(nexus-bench
    PRIVATE
        nexus::core
        nexus::sdk
)

target_compile_features(nexus-bench PRIVATE cxx_std_23)