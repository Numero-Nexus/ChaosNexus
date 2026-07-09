// Smoke test for nexus-core, verifying the GoogleTest + CTest +
// CMake testing infrastructure (Phase 7B). This intentionally tests
// only the Phase 7A bootstrap stub — no simulation engine logic
// exists yet, and this test must not introduce any.

#include "nexus-core/bootstrap.hpp"

#include <gtest/gtest.h>

namespace nexus::core::test {

TEST(BootstrapSmokeTest, ReturnsTrue)
{
    EXPECT_TRUE(bootstrap_ok());
}

} // namespace nexus::core::test