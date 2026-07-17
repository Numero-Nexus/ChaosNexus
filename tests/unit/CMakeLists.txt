# ==========================================
# tests/unit � Unit Test Suites
# ==========================================
#
# Mirrors the top-level nexus-* module structure so test ownership
# is unambiguous. Each subdirectory produces its own test executable,
# keeping failures scoped to the module under test. platform/ holds
# Core Utilities tests (Phase 8A onward) as its own top-level suite,
# sibling to core/sdk/plugins/utilities.

add_subdirectory(core)
add_subdirectory(sdk)
add_subdirectory(plugins)
add_subdirectory(utilities)
add_subdirectory(platform)
add_subdirectory(types)
add_subdirectory(logging)
add_subdirectory(error)
add_subdirectory(identity)
add_subdirectory(text)
