# ==========================================
# nexus-core — Deterministic Simulation Engine
# ==========================================
#
# This is a bootstrap placeholder for Phase 7A (Repository Bootstrap).
# No simulation engine logic exists yet. This target exists only to
# verify that the module compiles and links correctly within the
# overall CMake project structure.

add_library(nexus-core STATIC
    src/bootstrap.cpp
)

target_include_directories(nexus-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(nexus-core PUBLIC cxx_std_23)

add_library(nexus::core ALIAS nexus-core)