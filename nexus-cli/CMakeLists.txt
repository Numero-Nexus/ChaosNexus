# ==========================================
# nexus-cli — Operational Entry Point
# ==========================================
#
# This is a bootstrap placeholder for Phase 7A (Repository Bootstrap).
# No CLI logic exists yet. This target exists only to verify the
# executable builds and links against nexus-core and nexus-sdk.

add_executable(nexus-cli
    src/main.cpp
)

target_link_libraries(nexus-cli
    PRIVATE
        nexus::core
        nexus::sdk
)

target_compile_features(nexus-cli PRIVATE cxx_std_23)