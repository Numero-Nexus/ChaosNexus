// ==========================================
// nexus::core::time — VirtualClock
// ==========================================
//
// Sole authority for simulated time within a run (AD-4, SAD 8.4).
// Owns exactly one current TimePoint; every other subsystem queries
// current() read-only (SDS 10 "Time Ownership"). advance() is the
// only mutator, and takes an absolute target rather than a relative
// delta (ADR-0003).
//
// Phase 9 does not restrict which caller may invoke advance(). SDS
// 5.5 designates the future Scheduler (Phase 11) as the sole
// legitimate caller, but access control is deferred until the
// Scheduler's shape is known — see ADR-0003's Consequences and the
// Phase 10/11 handoff notes. This class has no extension points by
// design (SDS 5.5: "intentionally closed to extension").

#pragma once

#include "nexus/core/time/time_point.hpp"

namespace nexus::core::scheduler {
class Scheduler;
}
namespace nexus::core::time::testing {
struct VirtualClockTestAccess;
}

namespace nexus::core::time {

class VirtualClock {
public:
    /// Capability token restricting VirtualClock::advance() to its
    /// sole sanctioned caller. Only nexus::core::scheduler::Scheduler
    /// may construct one in production code (ADR-0007); a dedicated
    /// test-only friend exists solely to keep Phase 9 regression
    /// tests functional before the Scheduler exists.
    class AdvanceKey {
    public:
        AdvanceKey(const AdvanceKey&) = default;
        AdvanceKey(AdvanceKey&&) = default;

    private:
        AdvanceKey() = default;
        friend class nexus::core::scheduler::Scheduler;
        friend struct nexus::core::time::testing::VirtualClockTestAccess;
    };

    constexpr VirtualClock() noexcept = default;

    /// Returns the current virtual time. Safe to call from any
    /// subsystem at any point during a run (FR-015).
    [[nodiscard]] constexpr auto current() const noexcept -> TimePoint
    {
        return current_;
    }

    /// Advances the clock's current time to `target`. `target` must
    /// be greater than or equal to the current time (FR-015
    /// monotonicity; SDS 10). A smaller `target` is a contract
    /// violation (ADR-0004), not a recoverable error. `key` proves
    /// the caller is the sole sanctioned advancement authority
    /// (ADR-0007).
    auto advance(TimePoint target, AdvanceKey key) -> void;

private:
    TimePoint current_{TimePoint::epoch()};
};

} // namespace nexus::core::time
