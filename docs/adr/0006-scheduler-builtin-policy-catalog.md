# 0006 — Scheduler Built-In Policy Catalog

## Status
Accepted

## Context
FR-017 requires that scenario configuration select a documented
scheduling policy without requiring engine/plugin code changes. The
SRS, SAD, SDS, and Implementation Guide establish that a Scheduler
must support configuration-driven policy selection from a fixed,
documented built-in set, but none of these documents enumerate the
actual policy names, ordering rules, or fairness claims (Master
Prompt Section 19: "Do not assume FIFO or priority scheduling is
mandatory merely because the architecture gives them as examples").

Per user authorization (Step 11E), this ambiguity is resolved here
rather than left implicit or invented silently during implementation.

## Decision
The built-in scheduling-policy catalog is:

### `fifo` — First-In, First-Out
- **Ordering:** Delegates entirely to EventQueue's existing
  `(TimePoint, insertion_sequence)` order; adds no secondary key.
- **Fairness claim:** None. Strict submission-order service is not
  the same as a bounded-starvation guarantee under adversarial
  scenario construction.

### `priority` — Static Priority
- **Ordering:** `(TimePoint, priority descending, insertion_sequence
  ascending)`. Time remains primary and is never overridden; priority
  is a secondary key only among events already eligible at the same
  timestamp. Priority is caller-supplied data attached to a
  schedulable unit (via `assign_priority`), never hard-coded into
  Scheduler dispatch logic (SDS Section 11). Units with no assigned
  priority default to `Priority{0}`.
- **Fairness claim:** None. Static priority is explicitly
  starvation-prone: a persistent stream of higher-priority units can
  starve a lower-priority one indefinitely. This is a documented,
  accepted limitation, not an oversight.

### `round_robin` — Fair Round-Robin
- **Ordering:** Schedulable units are grouped by a caller-supplied
  Lane identifier (via `assign_lane`). The policy cycles through
  lanes in a fixed, deterministic rotation, selecting one eligible
  unit per lane per cycle among events eligible at the earliest
  pending timestamp.
- **Fairness claim:** Explicitly claims no indefinite starvation —
  every lane with a pending eligible unit is guaranteed service
  within one full rotation cycle, bounded by the number of active
  lanes at that time, for the duration of the scenario.

## Cross-Policy Rules
- No policy may select a later-eligible event ahead of an
  earlier-eligible one; EventQueue's temporal ordering is never
  violated in its time dimension by any policy.
- Every policy's selection is fully deterministic from `(resolved
  configuration, seed, pending event data, policy state)` alone — no
  pointer/hash/thread-order dependence (NFR-007).
- Only `round_robin` carries a fairness contract; `fifo` and
  `priority` are documented as explicitly not fairness-guaranteeing.

## Deliberately Excluded
- Random/weighted-random scheduling: not specified by any
  authoritative document; excluded per Master Prompt Section 25.
- Plugin-registered custom policies: deferred to the SDK
  extension-point catalog per SDS Section 5.4 and Master Prompt
  Section 18.
- Fairness claims for `fifo` or `priority`: would be false guarantees.

## Consequences
- Policy selection is a fixed, compile-time set (`if`/`else if` in
  `make_scheduler()`), not a runtime-registrable mechanism, matching
  the "fixed, documented" catalog scope established here. Extending
  the catalog later requires a new ADR.
- `priority` and `round_robin` require out-of-band data assignment
  (`assign_priority`/`assign_lane`) because Phase 10's `Event` carries
  no payload — this is a Scheduler-policy-level accommodation, not a
  change to the Event contract.
- Fairness testing (Step 11L) is obligated only for `round_robin`;
  `fifo`/`priority` are instead tested to confirm the *absence* of a
  fairness bound is real behavior, not merely an unclaimed flag.
