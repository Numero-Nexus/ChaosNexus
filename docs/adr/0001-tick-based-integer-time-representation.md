# 0001 — Tick-Based Signed 64-Bit Integer Time Representation

## Status

Accepted

## Context

Phase 9 (Virtual Time) requires an underlying representation for
`TimePoint` and `Duration`. The Architecture and Design documents
establish that the Virtual Clock is the sole time authority (SAD
§8.4, AD-4), must have zero dependency on wall-clock APIs (FR-013),
and must produce bit-identical results across repeated runs given
identical inputs (SDS §10 "Deterministic Guarantees", NFR-007).

Two realistic candidates were considered:

1. **`std::chrono`-based** (`std::chrono::duration`/`time_point`
   parameterized over some `rep`/`period`).
2. **A plain signed 64-bit integer tick count**, with no intrinsic
   unit.
3. **Floating-point time** (e.g. `double` seconds).

## Decision

`TimePoint` and `Duration` are backed by a signed 64-bit integer tick
count, using the project's existing `nexus::core::types::I64` alias
(see `include/nexus/core/types/types.hpp`) rather than a raw
`std::int64_t` or a `std::chrono` type.

## Consequences

- Arithmetic is exact, integer-only, and trivially reproducible
  across platforms and compilers — no `rep`/`period` template
  machinery, no implementation-defined rounding at conversion
  boundaries.
- Floating-point time is rejected outright: floating-point addition
  is not associative, which is incompatible with the bit-identical
  determinism guarantee (NFR-007, SDS §10).
- `std::chrono` is rejected as the representation because its
  generality (arbitrary `rep`/`period` combinations, implicit
  duration-cast conversions) works against the goal of a narrow,
  fully-controlled temporal domain — see ADR-0002 for how this
  interacts with the resolution/granularity question.
- The tick count has no intrinsic real-world unit; see ADR-0002.
- Reuses the project's existing fixed-width type vocabulary
  (`I64`), consistent with how `types.hpp` is used project-wide.
- Overflow/underflow of `I64` arithmetic must be explicitly checked
  (see ADR-0004) rather than relying on any implicit behavior.
