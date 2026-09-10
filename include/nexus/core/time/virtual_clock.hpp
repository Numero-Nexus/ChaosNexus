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

namespace nexus::core::time {

class VirtualClock {
public:
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
    /// violation (ADR-0004), not a recoverable error.
    auto advance(TimePoint target) -> void;

private:
    TimePoint current_{TimePoint::epoch()};
};

} // namespace nexus::core::time
