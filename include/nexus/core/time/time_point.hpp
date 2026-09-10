// ==========================================
// nexus::core::time — TimePoint
// ==========================================
//
// An absolute instant on a simulation's virtual timeline: "when," not
// "how much." TimePoint::epoch() (tick 0) is the simulation's start;
// negative TimePoint values are a contract violation (9E) since a
// simulation cannot have a point before its own start.
//
// Only the arithmetic explicitly authorized by the Phase 9 design is
// exposed: TimePoint +/- Duration, and TimePoint - TimePoint. There
// is no TimePoint + TimePoint (two absolute instants cannot be
// summed) and no implicit conversion to/from a raw integer.

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/time/detail/time_arithmetic.hpp"
#include "nexus/core/time/duration.hpp"
#include "nexus/core/types/types.hpp"

#include <compare>

namespace nexus::core::time {

class TimePoint {
public:
    constexpr TimePoint() noexcept = default;

    /// Constructs a TimePoint from a raw tick count. `ticks` must be
    /// non-negative (9E: the minimum representable TimePoint is the
    /// simulation epoch itself).
    [[nodiscard]] static auto from_ticks(types::I64 ticks) -> TimePoint
    {
        NEXUS_VERIFY_MSG(ticks >= 0, "TimePoint::from_ticks: negative TimePoint is invalid");
        return TimePoint{ticks};
    }

    [[nodiscard]] static constexpr auto epoch() noexcept -> TimePoint
    {
        return TimePoint{0};
    }

    [[nodiscard]] constexpr auto ticks() const noexcept -> types::I64
    {
        return ticks_;
    }

    [[nodiscard]] friend constexpr auto operator==(const TimePoint&, const TimePoint&) noexcept
        -> bool = default;

    [[nodiscard]] friend constexpr auto operator<=>(const TimePoint&, const TimePoint&) noexcept
        -> std::strong_ordering = default;

    [[nodiscard]] friend auto operator+(TimePoint lhs, Duration rhs) -> TimePoint
    {
        const auto result = detail::checked_add(lhs.ticks_, rhs.ticks());
        NEXUS_VERIFY_MSG(result >= 0, "TimePoint arithmetic result precedes simulation epoch");
        return TimePoint{result};
    }

    [[nodiscard]] friend auto operator-(TimePoint lhs, Duration rhs) -> TimePoint
    {
        const auto result = detail::checked_sub(lhs.ticks_, rhs.ticks());
        NEXUS_VERIFY_MSG(result >= 0, "TimePoint arithmetic result precedes simulation epoch");
        return TimePoint{result};
    }

    [[nodiscard]] friend auto operator-(TimePoint lhs, TimePoint rhs) -> Duration
    {
        return Duration::from_ticks(detail::checked_sub(lhs.ticks_, rhs.ticks_));
    }

private:
    explicit constexpr TimePoint(types::I64 ticks) noexcept : ticks_{ticks} {}

    types::I64 ticks_{0};
};

} // namespace nexus::core::time
