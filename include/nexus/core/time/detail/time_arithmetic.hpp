// ==========================================
// nexus::core::time::detail — Overflow-Checked Tick Arithmetic
// ==========================================
//
// Internal helpers shared by Duration and TimePoint for I64-tick
// arithmetic. Not part of the public Virtual Time API — do not
// include this header outside nexus/core/time/.
//
// Overflow/underflow are contract violations (ADR-0004), enforced
// via NEXUS_VERIFY rather than a recoverable Result, because a
// legitimate scenario should never come close to I64's range and
// silently allowing wraparound would corrupt determinism (NFR-007).

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/types/types.hpp"

#include <limits>

namespace nexus::core::time::detail {

/// Returns lhs + rhs, verifying the addition does not overflow the
/// range of types::I64. See ADR-0001, ADR-0004.
[[nodiscard]] inline auto checked_add(types::I64 lhs, types::I64 rhs) -> types::I64
{
    constexpr auto max = std::numeric_limits<types::I64>::max();
    constexpr auto min = std::numeric_limits<types::I64>::min();

    if (rhs >= 0) {
        NEXUS_VERIFY_MSG(lhs <= max - rhs, "Virtual Time arithmetic overflow");
    } else {
        NEXUS_VERIFY_MSG(lhs >= min - rhs, "Virtual Time arithmetic underflow");
    }

    return lhs + rhs;
}

/// Returns lhs - rhs, verifying the subtraction does not overflow the
/// range of types::I64. See ADR-0001, ADR-0004.
[[nodiscard]] inline auto checked_sub(types::I64 lhs, types::I64 rhs) -> types::I64
{
    constexpr auto max = std::numeric_limits<types::I64>::max();
    constexpr auto min = std::numeric_limits<types::I64>::min();

    if (rhs >= 0) {
        NEXUS_VERIFY_MSG(lhs >= min + rhs, "Virtual Time arithmetic underflow");
    } else {
        NEXUS_VERIFY_MSG(lhs <= max + rhs, "Virtual Time arithmetic overflow");
    }

    return lhs - rhs;
}

} // namespace nexus::core::time::detail
