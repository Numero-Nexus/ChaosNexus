# 0005 — Event Ordering: Timestamp Primary Key, Insertion-Sequence Tie-Break

## Status

Accepted

## Context

FR-007 requires the Event System to process events in a well-defined,
deterministic order consistent with virtual time, with ties resolved
by a documented, deterministic rule. `docs/06_VIRTUAL_TIME.md`'s
Phase 10 Handoff section explicitly states that same-timestamp
ordering is an Event System responsibility, not something Virtual
Time defines. SDS §9 establishes that every event carries "a virtual
timestamp and a causal-predecessor reference," but does not specify
the tie-break mechanism.

Three realistic candidates were considered for the same-timestamp
tie-break key:

1. **`EventId` ordering** — `EventId` wraps a `Uuid`, which is
   totally ordered by byte value (Phase 8).
2. **An internal, monotonically increasing insertion-sequence
   counter**, assigned at submission time.
3. **`EventType` ordering**, as a secondary sort key.

## Decision

Events are ordered by `(TimePoint, insertion_sequence)`, both
ascending. `TimePoint` is the primary key. Ties are broken by an
insertion-sequence number assigned internally by `EventQueue` at
`push()` time — not stored on `Event` itself, and not supplied by
the caller.

`EventId` and `EventType` play no role in ordering.

## Consequences

- `EventId` ordering was rejected because `Uuid` byte-value ordering
  has no semantic relationship to submission order; using it as a
  tie-break would make ordering deterministic but arbitrary relative
  to program behavior, and would make ordering silently depend on
  however IDs happen to be generated — exactly the kind of
  "incidental implementation detail" determinism guidance (Master
  Prompt §7 / NFR-007) warns against basing ordering on.
- `EventType` ordering was rejected because it would encode business
  meaning ("this kind of event always processes before that kind at
  the same instant") into what is meant to be an opaque
  discriminator, and would need revisiting the moment any concrete
  `EventType` catalog is introduced (Phase 11+/Plugin Manager).
- Insertion sequence directly encodes "first submitted, first
  processed among equal timestamps" — the natural, causality-
  respecting reading of FR-008, and matches how a Scheduler will
  submit events in practice.
- The sequence counter is `EventQueue`-owned state, not an `Event`
  field: it describes "position of submission into this queue," not
  an intrinsic property of the event value. This means `Event` itself
  does not define a complete ordering relation (no `operator<=>`);
  only `EventQueue`'s comparator does. A future component with a
  different insertion-ordering need (e.g. a second, independent
  queue) would assign its own sequence numbers rather than reusing
  a value baked into `Event`.
- The insertion-sequence counter is a `U64`, guarded by
  `NEXUS_VERIFY` against overflow, consistent with ADR-0004's
  precedent of treating theoretically-possible-but-unreachable
  invariant violations as contract violations rather than silent
  wraparound.
- If a future phase determines that ordering should additionally
  depend on causal-predecessor chains (rather than pure submission
  order) — for example if two events at the same timestamp must be
  ordered based on which one causally preceded the other rather than
  which was pushed first — that will require revisiting this ADR
  rather than silently diverging from it.

## Related Requirements

FR-007, FR-008, NFR-007, `docs/06_VIRTUAL_TIME.md` Phase 10 Handoff.
