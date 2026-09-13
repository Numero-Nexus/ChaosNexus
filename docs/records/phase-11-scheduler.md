# Phase 11 Technical Record — Scheduler

## 1. Technical Problem
Implement a deterministic, configuration-driven Scheduler subsystem
integrating Phase 9 VirtualClock and Phase 10 EventQueue, per FR-016
through FR-019 and NFR-007.

## 2. Authoritative Requirements
FR-016 (order/timing), FR-017 (config-driven policy selection),
FR-018 (fairness where claimed), FR-019 (reconstructable decisions),
NFR-007 (determinism).

## 3. Scheduler Architecture
`Scheduler` holds non-owning `VirtualClock&`/`EventQueue&`, an owned
`unique_ptr<SchedulingPolicy>`, its resolved `policy_name_`, and a
private `decision_sequence_` counter. Constructed via `make_scheduler()`
(free function, not a static member — ADR-0008, MSVC incomplete-type
restriction).

## 4. Policy Architecture
`SchedulingPolicy` abstract interface; three concrete policies
(`fifo`, `priority`, `round_robin`) per ADR-0006.

## 5. Policy Catalog
See ADR-0006 and `docs/08_SCHEDULER.md`.

## 6. Ordering Model
EventQueue temporal order is primary and inviolable; policies
reorder only within a same-timestamp batch. See `docs/08_SCHEDULER.md`
Known Limitations #1 for the re-queue insertion-sequence caveat.

## 7. Fairness Model
Only `round_robin` claims fairness (no indefinite starvation, bounded
by active lane count). Tested adversarially at both small (2-lane,
2-cycle) and scaled (5-lane, 100-selection sliding-window) levels
(Step 11L), after the initial small test was judged insufficient
against Master Prompt Section 24's bar.

## 8. VirtualClock Integration
ADR-0007 (`AdvanceKey` capability token). `Scheduler` is sole
production caller of `advance()`. Monotonicity enforced at both
`VirtualClock` (existing Phase 9 guard) and `Scheduler` (only calls
`advance()` when `selected.timestamp() > current()`) layers.

## 9. EventQueue Integration
Policies only use `push`/`pop`/`peek`/`empty`; no direct manipulation
of internal representation. Audited in Step 11J.

## 10. Deterministic Guarantees
Verified via full-trace equality across repeated identical runs for
all three policies (Step 11M, `RunTrace`/`run_fixed_scenario` harness).

## 11. Access-Control Mechanism
`VirtualClock::AdvanceKey` — private constructor, friended to
`Scheduler` (production) and `VirtualClockTestAccess` (test-only).

## 12. SchedulingDecision Model
See `docs/08_SCHEDULER.md`. Fields: `event_id`, `scheduled_timestamp`,
`policy_name`, `clock_advanced`, `sequence`.

## 13. Alternatives Considered
- `Scheduler::create()` static member returning `Result<Scheduler>`:
  rejected due to MSVC incomplete-type error on
  `move_constructible<Scheduler>` evaluated inside the class's own
  body (ADR-0008); replaced with free function `make_scheduler()`.
- Plugin-based policy registration: rejected per SDS Section 5.4
  deferral to SDK extension-point catalog (Master Prompt Section 18).
- Randomized scheduling policies: rejected, not specified in any
  authoritative document (Master Prompt Section 25).

## 14. Rejected Alternatives
See above.

## 15. Testing Evidence
290/290 tests passing (Debug and Release), including 21 new tests
across `FifoPolicy` (3), `PriorityPolicy` (4), `RoundRobinPolicy` (4),
`Scheduler` (10). Full breakdown in Steps 11H, 11K, 11L, 11M.

## 16. Known Limitations
See `docs/08_SCHEDULER.md` Known Limitations.

## 17. Phase 12 Handoff
See `docs/08_SCHEDULER.md` Phase 12 Handoff.
