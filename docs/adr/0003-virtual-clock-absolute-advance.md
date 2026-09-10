# 0003 — VirtualClock::advance() Takes an Absolute TimePoint Target

## Status

Accepted

## Context

AD-4 establishes the Virtual Clock as the single time authority; no
other subsystem may originate or advance simulated time. SDS §10
("Time Progression") states that virtual time "advances only in
response to the Scheduler's explicit request to advance to the next
event's scheduled time" — phrased as advancing *to* a target instant,
not *by* an elapsed span.

Two API shapes were considered for the Clock's mutator:

1. `advance(TimePoint target)` — absolute: move current time forward
   to a specific instant.
2. `advance(Duration delta)` — relative: move current time forward
   by a specific span.

## Decision

`VirtualClock::advance(TimePoint target)` takes an absolute target
`TimePoint`. There is no relative-advance overload in Phase 9.

## Consequences

- Matches SDS §10's literal phrasing ("advance to the next event's
  scheduled time") exactly — the Scheduler (Phase 11) will naturally
  hold a `TimePoint` (an event's timestamp) already, not a `Duration`
  it would otherwise have to compute relative to "now."
- Enables `advance()` to enforce monotonicity (ADR-0004) directly, by
  comparing `target` against the clock's current value, without the
  caller needing to separately track "current + delta" itself and
  risk divergence from the Clock's actual state.
- If a future phase genuinely needs relative advancement, it can be
  added as a convenience overload built on top of the absolute form
  (`advance(current() + delta)`) without breaking this contract.
- Phase 9 defines `advance()`'s signature and precondition but does
  not restrict *who* may call it (e.g. via `friend` or a capability
  token) because the Scheduler that SDS §5.5 designates as the sole
  legitimate caller does not exist yet (Phase 11). This is a
  deliberate, documented deferral: access-control on `advance()` is
  a required Phase 10/11 handoff item, not an oversight.
