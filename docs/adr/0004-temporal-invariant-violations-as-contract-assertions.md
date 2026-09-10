# 0004 — Temporal Invariant Violations Are Contract Assertions, Not New Error Codes

## Status

Accepted

## Context

Virtual Time has three classes of invalid operation to handle:

1. Backward clock advancement (`VirtualClock::advance(target)` called
   with `target < current()`), which would violate the monotonicity
   guarantee (FR-015 Acceptance Criteria, SDS §10).
2. Arithmetic overflow/underflow in `TimePoint`/`Duration` operations
   (ADR-0001's `I64` tick range is large, but not infinite).
3. Constructing a `Duration` from `I64::min()`, whose negation is not
   representable in `I64`.

The existing `ErrorCode` enum (`include/nexus/core/error/error_code.hpp`)
has no entry for any of these. Two approaches were considered:

1. Extend `ErrorCode` with new values (e.g. `Overflow`, `Underflow`,
   `NonMonotonicAdvance`) and report failures through the existing
   `Error`/`Result` machinery.
2. Treat all three as contract violations — conditions that can only
   arise from a caller bug, not from any legitimate scenario input —
   and enforce them via the project's existing `assert.hpp`
   mechanism, reusing `ErrorCode::PreconditionViolated`, which
   already exists.

## Decision

All three conditions are enforced as contract violations via
`NEXUS_VERIFY` (not `NEXUS_ASSERT`), which is unconditionally active
in both debug and release builds and throws
`InternalException{Error{ErrorCode::PreconditionViolated, ...}}` on
failure. No new `ErrorCode` values are added.

`NEXUS_VERIFY` is used rather than the debug-only `NEXUS_ASSERT`
because silently allowing a non-monotonic advance or an overflowed
tick count to pass through in a release build would corrupt
determinism (NFR-007) invisibly — this is exactly the always-active
tier `assert.hpp`'s own documentation describes as being for
conditions that must "fail safely instead of silently continuing."

## Consequences

- No competing error-reporting path is introduced for Virtual Time;
  it reuses the project's one existing distinction (assertions for
  programmer-error contracts vs. `Result`/`ErrorCode` for expected,
  recoverable runtime conditions such as bad configuration input).
- A non-monotonic `advance()` call, or an overflowing
  `TimePoint`/`Duration` arithmetic operation, throws
  `InternalException` rather than returning a `Result<T>` the caller
  could choose to ignore — reflecting that these are not conditions
  any correctly-functioning Scheduler (Phase 11) should ever trigger
  during normal operation.
- Because `NEXUS_VERIFY` remains active in release builds, this adds
  a small, bounded runtime check to every temporal arithmetic
  operation and every `advance()` call; this is accepted as
  necessary given determinism is this module's highest-priority
  quality attribute (Implementation Guide, "Virtual Clock" testing
  section).
- If a future phase determines that a temporal invariant violation
  should instead be a recoverable, caller-handleable condition (for
  example, if plugin-supplied scheduling logic should be allowed to
  fail gracefully rather than aborting the run), that will require
  revisiting this ADR rather than silently diverging from it.
