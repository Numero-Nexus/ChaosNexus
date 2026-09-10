# 0002 — Time Granularity Is a Scenario/Configuration Concern, Not a Type-Level Concern

## Status

Accepted

## Context

FR-014 requires the engine to support configuration of virtual time
granularity appropriate to the scenario being modeled, since
different scenarios (e.g. consensus protocols vs. large-scale
scheduling) require different time resolutions. Having chosen a
tick-based integer representation (ADR-0001), a decision is needed
on where "what does one tick mean" is expressed: baked into the
`TimePoint`/`Duration` types themselves (e.g. via a `std::chrono`-style
`Period` template parameter), or left external to those types.

## Decision

`TimePoint` and `Duration` are dimensionless at the type level — one
tick has no intrinsic real-world meaning (no nanoseconds, no
milliseconds). The mapping from "ticks" to any human-meaningful unit
a scenario cares about is a Configuration-subsystem concern (FR-014),
consumed by scenario/plugin logic, not something the Virtual Time
types encode or enforce.

## Consequences

- `TimePoint`/`Duration` stay minimal, reusable across arbitrarily
  different scenario domains, and require no template parameter or
  unit-conversion machinery.
- FR-014 ("configurable time granularity") is satisfied at the
  Configuration layer, not the Virtual Time layer — Virtual Time
  itself has nothing to configure.
- No unit-conversion arithmetic (e.g. tick-to-nanosecond) is part of
  Phase 9's scope; if a future phase needs it, it is layered on top
  of the tick count rather than into `TimePoint`/`Duration`.
- This intentionally departs from `std::chrono`'s `Period`-parameterized
  design (reinforcing ADR-0001's rejection of `std::chrono` as the
  representation) in favor of a simpler, single, unit-agnostic tick
  domain.
