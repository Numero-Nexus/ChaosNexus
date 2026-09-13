// ==========================================
// nexus-test-common — VirtualClock Test Access
// ==========================================
//
// Grants test code the capability to call VirtualClock::advance()
// directly, without depending on nexus::core::scheduler::Scheduler
// (which does not yet exist as of Phase 11 Step 11F). This is a
// test-only escape hatch: it is never included by nexus-core,
// nexus-sdk, or nexus-cli production sources, and it grants no
// runtime capability beyond what VirtualClock::AdvanceKey's friend
// declaration explicitly names (ADR-0007).

#pragma once

#include "nexus/core/time/virtual_clock.hpp"

namespace nexus::core::time::testing {

/// Test-only factory for VirtualClock::AdvanceKey. Friended by
/// VirtualClock itself; must not be used outside test targets.
struct VirtualClockTestAccess {
    [[nodiscard]] static auto make_key() -> VirtualClock::AdvanceKey
    {
        return VirtualClock::AdvanceKey{};
    }
};

} // namespace nexus::core::time::testing
