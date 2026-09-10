# Virtual Time (Phase 9)

## Overview

Virtual Time is the deterministic temporal foundation for the
ChaosNexus simulation engine. It establishes `TimePoint`, `Duration`,
and `VirtualClock` in namespace `nexus::core::time`, and is the sole
dependency the future Event System (Phase 10) and Scheduler
(Phase 11) will build on for ordering and timing.

## Concepts

- **`Duration`** — a relative span of virtual time ("how much").
- **`TimePoint`** — an absolute instant on the simulation timeline
  ("when"). `TimePoint::epoch()` (tick 0) is the simulation start;
  negative `TimePoint` values are invalid.
- **`VirtualClock`** — the sole authority for the current simulation
  time. Exposes `current()` (unrestricted, read-only) and
  `advance(TimePoint target)` (the only mutator).

## Representation

Both `TimePoint` and `Duration` are backed by a signed 64-bit integer
tick count (`nexus::core::types::I64`). A tick has no intrinsic
real-world unit — mapping ticks to a scenario-meaningful unit (e.g.
milliseconds) is a Configuration-layer concern, not a Virtual Time
concern. See ADR-0001, ADR-0002.

## Arithmetic

Only the following operations are supported:

TimePoint + Duration -> TimePoint
TimePoint - Duration -> TimePoint
TimePoint - TimePoint -> Duration
Duration + Duration -> Duration
Duration - Duration -> Duration


`TimePoint + TimePoint` does not compile — two absolute instants
cannot be summed.

## Determinism

Given identical operation sequences, `Duration`/`TimePoint`/
`VirtualClock` results are bit-identical by construction: pure
integer arithmetic, no wall-clock reads, no locale/iteration-order
dependency. See `tests/unit/time/test_time_determinism.cpp` and
`VirtualClock.DeterministicGivenIdenticalOperationSequence`.

## Error Handling

Monotonicity violations (`advance()` called with a target before the
current time) and arithmetic overflow/underflow are contract
violations, enforced via `NEXUS_VERIFY` (active in both Debug and
Release), reusing `ErrorCode::PreconditionViolated`. No new
`ErrorCode` values were introduced. See ADR-0004.

## Ownership

Every subsystem other than `VirtualClock` itself may only read
`current()`; only `VirtualClock::advance()` may change simulated
time. `advance()` currently has no caller access-control — this is
an intentional, documented deferral (see ADR-0003) until the
Scheduler (Phase 11) exists.

## Dependency Boundary

`nexus::core::time` depends only on `nexus::core::types` and
`nexus::core::error`. It has no dependency on any Phase 10+
component; none of Event System, Scheduler, Messaging, Observability,
Plugin Manager, SDK, CLI, Benchmarking, or Visualization exist in or
are referenced by this module.

## Phase 10 Handoff

The Event System may rely on:

- `TimePoint`'s total ordering (`operator<=>`) for event-timestamp
  comparison.
- `TimePoint`/`Duration` arithmetic for computing event delays and
  elapsed time.
- `VirtualClock::current()` for reading simulation time at any point.

The Event System must **not** rely on:

- `VirtualClock::advance()` being restricted to any particular caller
  — that restriction does not yet exist (Phase 10/11 must add it).
- Any tie-break rule for equal-timestamp `TimePoint`s — that is an
  Event System responsibility (FR-007), not something Virtual Time
  defines.

## See Also

- `docs/adr/0001-tick-based-integer-time-representation.md`
- `docs/adr/0002-scenario-level-time-resolution.md`
- `docs/adr/0003-virtual-clock-absolute-advance.md`
- `docs/adr/0004-temporal-invariant-violations-as-contract-assertions.md`
