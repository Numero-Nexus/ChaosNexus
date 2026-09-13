# ==========================================
# nexus-core — Deterministic Simulation Engine
# ==========================================
#
# Phase 7A bootstrap target, extended in Phase 8A to expose the
# top-level include/ root (Core Utilities convention — see
# 04_SOFTWARE_DESIGN_SPECIFICATION.md / Phase 8A ADR) alongside the
# original per-module include root, in Phase 10 to add the Event
# System, and in Phase 11 to add the Scheduler.

add_library(nexus-core STATIC
    src/bootstrap.cpp
    src/logging/log_format.cpp
    src/logging/console_sink.cpp
    src/logging/logger.cpp
    src/error/error_format.cpp
    src/identity/uuid_format.cpp
    src/text/strings.cpp
    src/config/config_key.cpp
    src/config/config_value.cpp
    src/hash/hash.cpp
    src/time/virtual_clock.cpp
    src/scheduler/scheduler.cpp
)

target_include_directories(nexus-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(nexus-core PUBLIC cxx_std_23)

add_library(nexus::core ALIAS nexus-core)
