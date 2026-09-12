# Event System (Phase 10)

## Overview

The Event System establishes the deterministic event abstraction
that the future Scheduler (Phase 11) and Simulation Kernel
(Phase 12) will build on. It introduces `Event`, `EventType`, and
`EventQueue` in namespace `nexus::core::events`, and is the sole
dependency the Scheduler will use for event representation and
deterministic ordering.

## Concepts

- **`Event`** — the uniform envelope every simulation event carries,
  regardless of origin: an identity (`EventId`), an absolute virtual
  timestamp (`TimePoint`), a type discriminator (`EventType`), and an
  optional causal-predecessor reference (`EventId`).
- **`EventType`** — an opaque, extensible discriminator for what kind
  of event an `Event` represents. Deliberately not a fixed enum, so
  plugin-defined event types (FR-009) are not precluded once a
  Plugin Manager exists.
- **`EventQueue`** — deterministic storage and retrieval of
  not-yet-dispatched `Event`s, ordered by `(timestamp,
  insertion_sequence)`. Not a Scheduler: it has no notion of virtual
  time advancement, dispatch, or execution policy.

## Identity

`Event` reuses the existing `EventId = StrongId<detail::EventTag>`
established in Phase 8 (`include/nexus/core/identity/ids.hpp`). No
new identity type was introduced. `EventId::nil()` denotes "no
causal predecessor" for a root/originating event.

## Ordering

Events are ordered by a strict total order: `(TimePoint,
insertion_sequence)`, both ascending. `TimePoint` is the primary key,
reusing Phase 9's total order (`operator<=>`). Same-timestamp ties
are broken by insertion sequence — a monotonically increasing counter
assigned internally by `EventQueue` at `push()` time.

`EventId` and `EventType` play **no role** in ordering. Payload (not
yet introduced — see Limitations) is likewise excluded. See
ADR-0005 for the full rationale and rejected alternatives.

The insertion-sequence counter is *not* a field of `Event` itself —
it is queue-owned state, describing "position of submission into
this queue," not an intrinsic property of the event value. `Event`
therefore exposes only `timestamp()`, not a complete ordering
relation; `EventQueue` is what composes the full order.

## Determinism

Given the same sequence of `push()` calls, `EventQueue::pop()`
yields the same sequence of events on every run, on every platform:
pure integer comparison (`TimePoint` ticks, `U64` sequence numbers),
no dependency on hash-table iteration order, pointer/address values,
or any other incidental detail. Verified in
`tests/unit/events/test_event_queue.cpp`
(`EventQueue.RepeatedIdenticalRunsProduceIdenticalExtractionOrder`,
`EventQueue.LargeSetExtractsInFullyDeterministicOrder`).

## Error Handling

Both contract violations introduced by the Event System —
constructing an `Event` with a nil `EventId`, and calling `peek()`
or `pop()` on an empty `EventQueue` — are enforced via
`NEXUS_VERIFY`, reusing `ErrorCode::PreconditionViolated`, following
the same precedent Phase 9 established (ADR-0004). No new
`ErrorCode` values were introduced. Insertion-sequence counter
overflow (`U64::max()` pushes in one run) is likewise a
`NEXUS_VERIFY` contract violation rather than a silent wraparound.

## Ownership

`Event` is an immutable, value-semantic type: all fields (identity,
timestamp, type, causal predecessor) are fixed at construction.
`EventQueue` owns every `Event` it holds by value; `push()` copies
an `Event` in, `pop()` copies it out and removes it from the queue,
and `peek()` returns a read-only reference that is invalidated by
any subsequent `push()`/`pop()` call.

## Limitations (Documented, Not Implemented)

- **Payload** is intentionally unspecified. No consumer (Scheduler,
  Messaging) exists yet to justify a concrete shape; adding one
  speculatively would violate the project's anti-speculative-
  abstraction principle. This is a Phase 11+ dependency.
- **Cancellation** (`Scheduled → Cancelled`, per the SDS §9 event
  lifecycle) is not implemented. Cancellation requires a reason to
  cancel (e.g. a partition event superseding a scheduled delivery),
  which is Messaging/Scheduler-level logic. This is a documented,
  intentional Phase 11+ dependency, not an oversight.
- **Dispatch, Recording, Completion** (the remainder of the SDS §9
  lifecycle beyond Created/Scheduled/Ordered) are out of Phase 10's
  scope entirely — they require a Scheduler and Observability
  subsystem that do not yet exist.
- `EventQueue::push()` does not deduplicate. Two pushes of a
  value-identical `Event` are two distinct queue entries, popped in
  insertion order at their shared timestamp. Deduplication policy is
  a Messaging/Scheduler-level concern, not an Event Queue one.

## Dependency Boundary

`nexus::core::events` depends only on `nexus::core::time`,
`nexus::core::identity`, `nexus::core::error`, and
`nexus::core::types`. It has no dependency on any Phase 11+
component; none of Scheduler, Simulation Kernel, Messaging,
Observability, Plugin Manager, SDK, CLI, Benchmarking, or
Visualization exist in or are referenced by this module.

## Phase 11 Handoff

The Scheduler may rely on:

- `Event`'s immutable envelope (`id()`, `timestamp()`, `type()`,
  `causal_predecessor()`, `has_causal_predecessor()`).
- `EventQueue::push()`/`peek()`/`pop()`/`empty()`/`size()` and the
  `(timestamp, insertion_sequence)` total order they guarantee.
- The insertion-sequence tie-break being stable: events pushed
  earlier at an equal timestamp are always popped first.

The Scheduler must **not** rely on:

- Any payload shape — none exists yet; the Scheduler (or a
  subsequent phase) must define and add it.
- Any cancellation mechanism — `EventQueue` has no `cancel()`
  operation; if the Scheduler needs to remove a not-yet-dispatched
  event, that capability must be designed and added as part of
  Phase 11, not assumed to already exist.
- `EventType`'s raw representation as a stable, externally-defined
  catalog — Phase 10 only guarantees it as an opaque, comparable
  discriminator; assigning concrete meaning to specific values is a
  Phase 11+/Plugin Manager responsibility.
- Any restriction on who may construct an `EventQueue` or how many
  may exist — Phase 10 does not constrain this; if Phase 11 requires
  exactly one queue per Scenario Runner, that constraint must be
  enforced by the Scheduler/Simulation Engine composition, not
  assumed to be enforced here.

## See Also

- `docs/adr/0005-event-ordering-insertion-sequence-tiebreak.md`
- `docs/records/phase-10-event-system.md`
