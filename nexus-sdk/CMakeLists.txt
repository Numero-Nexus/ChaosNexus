# ==========================================
# nexus-sdk — Extension Interface Layer
# ==========================================
#
# This is a bootstrap placeholder for Phase 7A (Repository Bootstrap).
# nexus-sdk is header-only by design: it defines the contract through
# which nexus-core is extended, without exposing implementation detail.
# No SDK surface is defined yet.

add_library(nexus-sdk INTERFACE)

target_include_directories(nexus-sdk
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(nexus-sdk INTERFACE cxx_std_23)

target_link_libraries(nexus-sdk INTERFACE nexus::core)

add_library(nexus::sdk ALIAS nexus-sdk)