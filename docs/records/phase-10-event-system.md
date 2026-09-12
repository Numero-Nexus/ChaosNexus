# Technical Record — Phase 10: Event System

1. **Technical problem**: The Scheduler (Phase 11) and Simulation
   Kernel (Phase 12) need a deterministic event abstraction and
   ordering mechanism before they can be built (FR-007–FR-011,
   SAD §8.2/§9/§11, SDS §5.3/§9).

2. **Requirements**: FR-007 (deterministic ordering), FR-008
   (causality), FR-009 (plugin-extensible event types, non-precluded
   only — no Plugin Manager exists yet), FR-011 (introspection,
   non-precluded only — no Observability module exists yet). Sourced
   from SRS §9.2, SAD §8.2/§9/§11, SDS §5.3/§9, and
   `docs/06_VIRTUAL_TIME.md`'s Phase 10 Handoff.

3. **Selected event model**: Three-entity model — `Event`,
   `EventType`, `EventQueue` — in namespace `nexus::core::events`.

4. **Identity**: Reused the existing `EventId = StrongId<EventTag>`
   from Phase 8 (`include/nexus/core/identity/ids.hpp`). No new
   identity type introduced.

5. **Timestamp**: Reused `nexus::core::time::TimePoint` (Phase 9) as
   the sole ordering-relevant field on `Event`.

6. **Ordering**: `(TimePoint, insertion_sequence)`, both ascending;
   insertion sequence is `EventQueue`-owned state, not an `Event`
   field. ADR-0005.

7. **Type discriminator**: `EventType` — an opaque `U32`-backed
   value, not a fixed enum, to avoid precommitting to business-
   meaningful event kinds ahead of Scheduler/Messaging/Plugin phases.

8. **Invariants**: `Event::id()` is never nil (enforced at
   construction); `EventQueue::peek()`/`pop()` require a non-empty
   queue; `EventQueue` extraction order is a strict total order
   (no two entries share `(timestamp, insertion_sequence)`).

9. **Error handling strategy**: Reused Phase 9's ADR-0004 precedent
   — invariant violations (nil `EventId`, empty-queue access,
   insertion-sequence overflow) are contract violations via
   `NEXUS_VERIFY`, reusing `ErrorCode::PreconditionViolated`. No new
   `ErrorCode` values were introduced.

10. **Queue data structure**: Binary heap over `std::vector<Entry>`
    (`std::ranges::push_heap`/`pop_heap`), giving O(log n)
    push/pop and O(1) peek. Standard-library only, per the
    Implementation Guide §9 dependency policy.

11. **Determinism guarantees**: Bit-identical extraction order for
    identical push sequences, verified in
    `EventQueue.RepeatedIdenticalRunsProduceIdenticalExtractionOrder`
    and `EventQueue.LargeSetExtractsInFullyDeterministicOrder`;
    structurally guaranteed by the absence of hash-based iteration,
    pointer-address dependence, or wall-clock dependence anywhere in
    the ordering comparator.

12. **Alternatives considered**: `EventId`-based tie-break (rejected
    — ADR-0005); `EventType`-based tie-break (rejected — ADR-0005);
    storing insertion sequence on `Event` itself (rejected in favor
    of queue-owned state — ADR-0005); new `ErrorCode` values for
    Event/Queue contract violations (rejected — reused
    `PreconditionViolated` per ADR-0004 precedent); `std::priority_queue`
    as the backing structure (rejected in favor of direct heap
    algorithms over `std::vector`, to preserve a genuine `const
    Event&` from `peek()`).

13. **Rejected alternatives**: See ADR-0005's Context/Decision.

14. **Implementation consequences**: `nexus::core::events` depends
    only on `nexus::core::time`, `nexus::core::identity`,
    `nexus::core::error`, `nexus::core::types`. The future Scheduler
    (Phase 11) must define event payload representation and any
    cancellation mechanism — both documented gaps, not oversights.

15. **Testing evidence**: 23 Phase 10 tests (9 Event, 4 EventType,
    10 EventQueue) — all passing in Debug (`ctest`, this session).
    Full regression suite: 269/269 passing, confirming zero Phase
    1–9 regressions.

16. **Static analysis**: clang-format clean. clang-tidy reports only
    pre-existing, non-attributable findings shared with Phase 9's
    `duration.hpp`/`time_point.hpp`/`time_arithmetic.hpp` (exceptions-
    disabled/`avoid-do-while` diagnostics originating from the
    project-wide `NEXUS_VERIFY` macro, not from Phase 10 code). All
    Phase-10-attributable findings (designated initializers,
    unnecessary `std::move` on a trivially-copyable type) were fixed.

17. **Build evidence**: Debug and Release builds both complete
    cleanly with no new warnings beyond the pre-existing,
    project-wide `C4530` (`/EHsc`) note already present since
    Phase 7/8.

18. **Future-phase dependencies**: Documented in the Phase 11
    Handoff section of `docs/07_EVENT_SYSTEM.md`.
