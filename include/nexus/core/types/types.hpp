// ==========================================
// nexus::core::types — Common Type Aliases
// ==========================================
//
// Centralized, project-wide type aliases used throughout ChaosNexus:
// fixed-width integers, floating-point types, byte types, and
// size/index types. Every future subsystem depends, directly or
// indirectly, on the aliases defined here, so this header keeps the
// project's numeric vocabulary consistent instead of leaving each
// subsystem to invent its own.
//
// This header intentionally avoids introducing aliases that merely
// rename a standard library type without adding consistency or
// readability value (e.g. it does not alias std::string, since
// "String" would say nothing that std::string does not already say).
// Aliases here exist because ChaosNexus's numeric vocabulary
// (fixed-width integers, byte types, index types) benefits from a
// single, explicit, project-wide spelling.
//
// This is strictly a compile-time, zero-cost facility. There is no
// runtime state, no allocation, and no dependency on anything outside
// the standard library.
//
// Layering: this header is part of the Core Utilities layer and sits
// at the dependency floor of the entire project, alongside
// nexus/core/platform/platform.hpp. It must never depend on the
// Simulation Engine, SDK, CLI, or Plugins. Higher layers may freely
// depend on it.

#pragma once

#include <cstddef>
#include <cstdint>

namespace nexus::core::types
{

// ------------------------------------------
// Fixed-width signed integer aliases
// ------------------------------------------

using I8 = std::int8_t;
using I16 = std::int16_t;
using I32 = std::int32_t;
using I64 = std::int64_t;

// ------------------------------------------
// Fixed-width unsigned integer aliases
// ------------------------------------------

using U8 = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

// ------------------------------------------
// Floating-point aliases
// ------------------------------------------

using F32 = float;
using F64 = double;

static_assert(sizeof(F32) == 4, "F32 must be exactly 32 bits wide.");
static_assert(sizeof(F64) == 8, "F64 must be exactly 64 bits wide.");

// ------------------------------------------
// Byte aliases
// ------------------------------------------

/// A single, unsigned octet. Distinct from `char`/`unsigned char` in
/// spelling only; used wherever code means "one byte of data" rather
/// than "one character of text", matching std::byte's own intent.
using Byte = std::byte;

/// An unsigned octet usable in arithmetic contexts. `std::byte`
/// deliberately supports no arithmetic or implicit integer
/// conversions, so U8 remains the correct choice whenever a byte-sized
/// value participates in arithmetic (checksums, counters, bit
/// manipulation); Byte is reserved for opaque, non-arithmetic byte
/// data (buffers, serialized payloads).
using RawByte = U8;

// ------------------------------------------
// Size and index aliases
// ------------------------------------------

/// The project-wide alias for sizes and object counts. Identical to
/// std::size_t; declared explicitly so subsystems reference a single
/// project-owned name rather than the standard library type directly,
/// keeping a consistent vocabulary across the codebase.
using Size = std::size_t;

/// The project-wide alias for signed differences between positions
/// (iterator distances, offsets that may be negative).
using Diff = std::ptrdiff_t;

/// The project-wide alias for non-negative indices into a sequence.
/// Distinct from Size in intent: Size denotes "how many", Index
/// denotes "which one". Both share the same underlying type today,
/// but keeping them distinct in name allows the two concepts to
/// diverge later without a project-wide rename.
using Index = std::size_t;

// ------------------------------------------
// Compile-time helper traits
// ------------------------------------------

namespace detail
{

/// Maps a byte width to the corresponding fixed-width unsigned
/// integer alias. Used internally to derive UOfSize/IOfSize without
/// duplicating the width-to-type mapping.
template <Size Width>
struct UnsignedOfSize;

template <>
struct UnsignedOfSize<1>
{
    using type = U8;
};

template <>
struct UnsignedOfSize<2>
{
    using type = U16;
};

template <>
struct UnsignedOfSize<4>
{
    using type = U32;
};

template <>
struct UnsignedOfSize<8>
{
    using type = U64;
};

template <Size Width>
struct SignedOfSize;

template <>
struct SignedOfSize<1>
{
    using type = I8;
};

template <>
struct SignedOfSize<2>
{
    using type = I16;
};

template <>
struct SignedOfSize<4>
{
    using type = I32;
};

template <>
struct SignedOfSize<8>
{
    using type = I64;
};

} // namespace detail

/// Resolves to the fixed-width unsigned integer type whose size in
/// bytes is exactly `Width` (one of 1, 2, 4, 8).
template <Size Width>
using UOfSize = typename detail::UnsignedOfSize<Width>::type;

/// Resolves to the fixed-width signed integer type whose size in
/// bytes is exactly `Width` (one of 1, 2, 4, 8).
template <Size Width>
using IOfSize = typename detail::SignedOfSize<Width>::type;

} // namespace nexus::core::types
