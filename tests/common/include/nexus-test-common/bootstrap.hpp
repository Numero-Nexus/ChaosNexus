// nexus-test-common bootstrap header.
//
// This header exists to verify the nexus-test-common module's
// include path is correctly wired during Phase 7B (Testing
// Infrastructure). Real shared test utilities (custom assertions,
// helper functions) will be added as future test suites need them.

#pragma once

namespace nexus::test_common {

// Build-verification anchor only.
auto bootstrap_ok() noexcept -> bool;

} // namespace nexus::test_common