# ==========================================
# tests/unit — Unit Test Suites
# ==========================================
#
# Mirrors the top-level nexus-* module structure so test ownership
# is unambiguous. Each subdirectory produces its own test executable,
# keeping failures scoped to the module under test.

add_subdirectory(core)
add_subdirectory(sdk)
add_subdirectory(plugins)
add_subdirectory(utilities)