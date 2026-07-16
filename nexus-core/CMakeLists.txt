# ==========================================
# nexus-core — Deterministic Simulation Engine
# ==========================================
#
# Phase 7A bootstrap target, extended in Phase 8A to expose the
# top-level include/ root (Core Utilities convention — see
# 04_SOFTWARE_DESIGN_SPECIFICATION.md / Phase 8A ADR) alongside the
# original per-module include root. No simulation engine logic exists
# yet; this target still exists primarily to prove the module compiles
# and links correctly, now carrying real Core Utilities content
# (Platform Utilities) as of Phase 8A.

add_library(nexus-core STATIC
    src/bootstrap.cpp
    src/logging/log_format.cpp
    src/logging/console_sink.cpp
    src/logging/logger.cpp
)

target_include_directories(nexus-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(nexus-core PUBLIC cxx_std_23)

add_library(nexus::core ALIAS nexus-core)
