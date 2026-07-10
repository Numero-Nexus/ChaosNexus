// ==========================================
// nexus::core::types — Common Concepts
// ==========================================
//
// Compile-time concepts used throughout ChaosNexus to constrain
// templates in a consistent, readable way: signed/unsigned integral
// constraints, general arithmetic constraints, trivially-copyable
// constraints, and an enum helper concept. These build on the
// aliases in types.hpp and on <concepts>/<type_traits>.
//
// Layering: this header is part of the Core Utilities layer. It must
// never depend on the Simulation Engine, SDK, CLI, or Plugins.

#pragma once

#include "types.hpp"

#include <concepts>
#include <type_traits>

namespace nexus::core::types
{

/// Satisfied by any signed integral type (int8_t..int64_t, int, long,
/// etc.), but not by bool. std::signed_integral<bool> is false already,
/// so no extra exclusion is needed here — unlike UnsignedIntegral below.
template <typename T>
concept SignedIntegral = std::signed_integral<T> && !std::same_as<T, bool>;

/// Satisfied by any unsigned integral type (uint8_t..uint64_t,
/// size_t, etc.), but not by bool. std::unsigned_integral<bool> is
/// true per the standard's concept definition (bool is integral and
/// not signed), so bool must be excluded explicitly to keep this
/// concept consistent with Integral/SignedIntegral.
template <typename T>
concept UnsignedIntegral = std::unsigned_integral<T> && !std::same_as<T, bool>;

/// Satisfied by any integral type, signed or unsigned, excluding
/// bool.
template <typename T>
concept Integral = (std::integral<T> && !std::same_as<T, bool>);

/// Satisfied by any floating-point type (float, double, long double).
template <typename T>
concept FloatingPoint = std::floating_point<T>;

/// Satisfied by any arithmetic type: integral (excluding bool) or
/// floating-point.
template <typename T>
concept Arithmetic = Integral<T> || FloatingPoint<T>;

/// Satisfied by any scoped or unscoped enumeration type.
template <typename T>
concept Enum = std::is_enum_v<T>;

/// Satisfied by scoped ("enum class") enumerations only, excluding
/// legacy unscoped enums that implicitly convert to int.
template <typename T>
concept ScopedEnum = Enum<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;

/// Satisfied by any type that is trivially copyable: safe to copy via
/// memcpy, safe to serialize byte-for-byte, no user-defined copy
/// semantics required.
template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

/// Satisfied by any type that is both trivially copyable and
/// standard-layout — i.e. safe to reason about in terms of a plain
/// byte layout matching the C++ memory model's guarantees.
template <typename T>
concept TriviallyLayoutCompatible = TriviallyCopyable<T> && std::is_standard_layout_v<T>;

} // namespace nexus::core::types
