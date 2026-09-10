// ==========================================
// nexus::core::time — Duration
// ==========================================
//
// A relative span of virtual time: "how much time," not "when."
// Backed by a signed I64 tick count (ADR-0001), dimensionless at the
// type level (ADR-0002 — one tick has no intrinsic real-world unit;
// that mapping belongs to the Configuration subsystem, per FR-014).
//
// Only the arithmetic explicitly authorized by the Phase 9 design is
// exposed: Duration + Duration and Duration - Duration. There is no
// multiplication, no division, and no implicit conversion to/from a
// raw integer — construction and extraction both go through named,
// explicit functions (from_ticks / ticks) to prevent accidental
// mixing of ticks with any other numeric quantity.

#pragma once

#include "nexus/core/time/detail/time_arithmetic.hpp"
#include "nexus/core/types/types.hpp"

#include <compare>
#include <limits>

namespace nexus::core::time {

class Duration {
public:
    constexpr Duration() noexcept = default;

    /// Constructs a Duration from a raw tick count. `ticks` must not
    /// be I64::min() (ADR-0001/9F: its negation is not representable
    /// in I64, so it is disallowed as a constructible value even
    /// though this type does not itself expose negation).
    [[nodiscard]] static auto from_ticks(types::I64 ticks) -> Duration
    {
        NEXUS_VERIFY_MSG(ticks != std::numeric_limits<types::I64>::min(),
                         "Duration::from_ticks: I64::min() is not a representable Duration");
        return Duration{ticks};
    }

    [[nodiscard]] static constexpr auto zero() noexcept -> Duration
    {
        return Duration{0};
    }

    [[nodiscard]] constexpr auto ticks() const noexcept -> types::I64
    {
        return ticks_;
    }

    [[nodiscard]] friend constexpr auto operator==(const Duration&, const Duration&) noexcept
        -> bool = default;

    [[nodiscard]] friend constexpr auto operator<=>(const Duration&, const Duration&) noexcept
        -> std::strong_ordering = default;

    [[nodiscard]] friend auto operator+(Duration lhs, Duration rhs) -> Duration
    {
        return Duration{detail::checked_add(lhs.ticks_, rhs.ticks_)};
    }

    [[nodiscard]] friend auto operator-(Duration lhs, Duration rhs) -> Duration
    {
        return Duration{detail::checked_sub(lhs.ticks_, rhs.ticks_)};
    }

private:
    explicit constexpr Duration(types::I64 ticks) noexcept : ticks_{ticks} {}

    types::I64 ticks_{0};
};

} // namespace nexus::core::time
