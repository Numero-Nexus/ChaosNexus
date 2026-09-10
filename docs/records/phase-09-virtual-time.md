# Technical Record — Phase 9: Virtual Time

1. **Technical problem**: The simulation engine needs a deterministic,
   wall-clock-independent notion of time before an Event System or
   Scheduler can be built (FR-012–FR-015, AD-4).

2. **Requirements**: See Step 9A requirements matrix (R1–R19),
   sourced from SRS FR-012–FR-015, SAD AD-4/§8.4, SDS §5.5/§10, and
   the Implementation Guide's Milestone 3 exit criterion.

3. **Selected time model**: Three-entity model — `TimePoint`,
   `Duration`, `VirtualClock` — with the closed arithmetic set
   defined in Step 9B.

4. **Representation**: Signed 64-bit integer tick count
   (`nexus::core::types::I64`). ADR-0001.

5. **Resolution**: Dimensionless at the type level; scenario-specific
   meaning is a Configuration-layer concern. ADR-0002.

6. **Invariants**: `TimePoint >= epoch()` (non-negative); `VirtualClock`
   current time is monotonically non-decreasing.

7. **Arithmetic semantics**: `TimePoint +/- Duration -> TimePoint`,
   `TimePoint - TimePoint -> Duration`, `Duration +/- Duration ->
   Duration`. All overflow-checked.

8. **Overflow strategy**: Contract violation via `NEXUS_VERIFY`
   (always-active), not saturation/wraparound, not a recoverable
   `Result`. ADR-0004.

9. **Clock semantics**: `current()` read-only, unrestricted;
   `advance(TimePoint target)` absolute-target, sole mutator. ADR-0003.

10. **Determinism guarantees**: Bit-identical results for identical
    operation sequences, verified across 1000 repeated iterations in
    `TimeDeterminism.RepeatedManyTimesRemainsStable`, and structurally
    guaranteed by the absence of floating-point, wall-clock, or
    iteration-order dependencies.

11. **Alternatives considered**: `std::chrono`-based representation
    (rejected — ADR-0001); relative-delta `advance(Duration)` API
    (rejected — ADR-0003); `ErrorCode` extension for temporal failures
    (rejected — ADR-0004); type-level resolution parameterization
    (rejected — ADR-0002).

12. **Rejected alternatives**: See each ADR's Context/Decision.

13. **Implementation consequences**: `nexus::core::time` is a purely
    Core-Utilities-dependent module; the future Scheduler must add
    caller access-control to `advance()` (documented gap, not yet
    implemented).

14. **Testing evidence**: 39 Phase 9 tests (9 Duration, 12 TimePoint,
    6 VirtualClock, 4 invariant/property, 3 determinism, 5
    parameterized monotonicity cases) — all passing in both Debug and
    Release (`ctest` output, Steps 9S–9X). Full regression suite:
    246/246 passing (Step 9V), confirming zero Phase 8 regressions.

15. **Future-phase dependencies**: Documented in the Phase 10 Handoff
    section of `docs/06_VIRTUAL_TIME.md`.
