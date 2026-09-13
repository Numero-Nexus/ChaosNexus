# 08 — Scheduler

## Responsibility
Determines execution ORDER and TIMING of simulated task execution
(FR-016). Does not determine task content or internal behavior
(Master Prompt Section 3). Depends on VirtualClock (Phase 9) and
EventQueue (Phase 10); owns policy selection and policy-local state
only.

## Policy Model (ADR-0006)
Strategy interface `SchedulingPolicy` with three built-in
implementations, selected by configuration name via `make_scheduler()`
(ADR-0008):

| Policy         | Ordering                                              | Fairness claim |
|----------------|--------------------------------------------------------|----------------|
| `fifo`         | Delegates entirely to EventQueue's (timestamp, insertion_sequence) order | None |
| `priority`     | Highest caller-assigned Priority among same-timestamp batch; ties by arrival order | None — starvation-prone by design |
| `round_robin`  | Fixed rotation across caller-assigned Lanes among same-timestamp batch | Yes — no indefinite starvation, bounded by active lane count |

Priority and Lane are supplied out-of-band (`assign_priority`,
`assign_lane`), since Phase 10's Event carries no payload.

## Ordering and Precedence
EventQueue's `(TimePoint, insertion_sequence)` order is the temporal
authority and is never overridden across distinct timestamps by any
policy. Policies only reorder *within* the batch of events sharing the
single earliest pending timestamp. `priority`/`round_robin` pop and
re-push non-selected same-timestamp events, which assigns them a new
EventQueue insertion sequence — a documented limitation (see below),
not a violation of NFR-007.

## VirtualClock Integration (ADR-0007)
`Scheduler` is the sole production holder of `VirtualClock::AdvanceKey`
and the sole originator of clock-advancement requests. `advance_and_select()`
reads `clock.current()`, selects via the configured policy, advances
the clock only if the selected timestamp exceeds current time, and
returns both the selected `Event` and a `SchedulingDecision`.

## Scheduling Decision Model (FR-019)
`SchedulingDecision` records: `event_id`, `scheduled_timestamp`,
`policy_name`, `clock_advanced`, and a Scheduler-owned monotonic
`sequence` counter (distinct from EventQueue's insertion sequence).
This is a passive data contract only — Phase 11 does not implement
Observability, logging, or serialization (Master Prompt Section 32).

## Error Handling
`ErrorCode::UnknownSchedulingPolicy` (category: Configuration) is
returned via `Result<Scheduler>` from `make_scheduler()` for an
unresolvable policy name — a recoverable configuration error, not a
`NEXUS_VERIFY` contract violation.

## Known Limitations
1. Re-queued non-selected same-timestamp events receive a new
   EventQueue insertion sequence, which can reorder them relative to
   events pushed by another caller between the original push and the
   `select_next()` call. Deterministic per fixed call sequence; not
   fixed in Phase 11 (would require new EventQueue API surface, out of
   scope — Master Prompt Section 40).
2. `fifo` and `priority` provide no fairness bound; only `round_robin`
   does, and its bound is `O(active lane count)`, not a hard constant.
3. No plugin-based policy registration exists yet (deferred to SDK
   extension-point catalog per SDS Section 5.4).

## Phase 12 Handoff
Simulation Kernel may rely on: `make_scheduler()`'s Result-based error
contract, `advance_and_select()`'s `(Event, SchedulingDecision)` pair,
and the guarantee that VirtualClock only ever advances to a selected
event's own timestamp. Simulation Kernel must not assume: cancellation
support, a stable EventType catalog, or preservation of original
insertion order for re-queued same-timestamp events under `priority`/
`round_robin`.
