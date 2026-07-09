# ==========================================
# nexus-lab — Visualization & Exploration Environment
# ==========================================
#
# This is a bootstrap placeholder for Phase 7A (Repository Bootstrap).
# No visualization logic exists yet. This target exists only to verify
# the executable builds and links against nexus-core and nexus-sdk.

add_executable(nexus-lab
    src/main.cpp
)

target_link_libraries(nexus-lab
    PRIVATE
        nexus::core
        nexus::sdk
)

target_compile_features(nexus-lab PRIVATE cxx_std_23)