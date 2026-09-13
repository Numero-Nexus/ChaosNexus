# 0007 — VirtualClock::advance() Access Control via Capability Token

## Status
Accepted

## Context
Phase 9 intentionally left `VirtualClock::advance()` without
caller-level access control, explicitly deferring enforcement until
the Scheduler subsystem exists (ADR-0003's Consequences; SAD/SDS
state the Scheduler is the sole originator of requests to advance
simulation time). Phase 11 Step 11F must resolve this deferred
architectural decision before or alongside the Scheduler API itself,
per Master Prompt Section 17: "Do NOT merely document that Scheduler
'should' be the only caller if the architecture requires
enforcement."

## Decision
`VirtualClock::advance()` requires a non-forgeable capability token,
`VirtualClock::AdvanceKey`, passed by value as an additional
parameter: `advance(TimePoint target, AdvanceKey key)`.

`AdvanceKey`:
- Has a private default constructor.
- Is copy/move-constructible (so callers can receive and pass it),
  but not independently constructible outside its friends.
- Is friended to exactly two entities: `nexus::core::scheduler::Scheduler`
  (the sole production-sanctioned caller) and
  `nexus::core::time::testing::VirtualClockTestAccess` (a test-only
  factory, needed because Phase 9's regression suite predates the
  Scheduler's existence and must keep calling `advance()` directly).

This is the standard C++ "passkey idiom": a zero-runtime-cost,
compile-time-enforced capability mechanism. It requires no RTTI, no
virtual dispatch, and no new `ErrorCode`.

## Consequences
- `VirtualClock::advance()`'s signature changes from `advance(TimePoint)`
  to `advance(TimePoint, AdvanceKey)`, requiring every existing call
  site (Phase 9's `test_virtual_clock.cpp`, `test_time_invariants.cpp`)
  to be migrated to supply a key via the new
  `VirtualClockTestAccess::make_key()` test helper.
- Production code outside `Scheduler` cannot call `advance()` at all —
  attempting to do so is a compile-time error, not a runtime check,
  satisfying AD-4 ("Scheduler is the sole originator of requests to
  advance to the next simulation time") with the strongest available
  guarantee.
- `VirtualClockTestAccess` lives in `tests/common/` and is never
  included by any production target (`nexus-core`, `nexus-sdk`,
  `nexus-cli`); it exists solely to avoid coupling Phase 9's test
  suite to the Scheduler's existence.
- `AdvanceKey` intentionally does not define a destructor or
  assignment operators beyond the implicit ones — it is a stateless,
  empty type, and Rule-of-Five boilerplate would add no safety value
  for a capability token with no owned resources.
- This does not alter `VirtualClock`'s existing monotonicity guard
  (`NEXUS_VERIFY_MSG(target >= current_, ...)`, ADR-0004); the key
  parameter is purely an authorization gate, orthogonal to the
  existing value-correctness check.
