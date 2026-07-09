# ChaosNexus — Future Test Strategy

## Purpose

This document defines *how* future subsystems should be tested once
they are implemented in later phases. It does not implement any of
these tests — per Phase 7B's non-objectives, no framework logic
(Scheduler, Virtual Clock, Simulation Engine, Plugin Manager, SDK,
CLI) exists yet.

---

## Core Utilities

- Pure unit tests in `tests/unit/utilities/`.
- Since utility code (containers, math helpers, string/formatting
  helpers) is typically stateless or trivially stateful, favor plain
  `TEST()` over fixtures.
- Property-style testing (e.g. round-trip serialization, invariant
  checks across a range of inputs) is appropriate here once a
  suitable approach is chosen — GoogleTest's `TYPED_TEST`/
  `INSTANTIATE_TEST_SUITE_P` can cover parameterized cases without
  introducing a new dependency.

## Virtual Clock

- `tests/unit/core/` (or a dedicated subdirectory if it grows large).
- This is the highest-priority subsystem to get right, since
  determinism (Vision §8, §11) depends on it end-to-end.
- Tests must verify: monotonicity, correct ordering of simultaneous
  events (a defined tie-breaking rule), and that advancing the clock
  produces bit-identical results across repeated runs given the same
  inputs (see Testing Standards §8, §10).
- A dedicated `VirtualClockFixture` belongs in `tests/fixtures/` once
  more than one test suite needs a pre-configured clock instance.

## Scheduler

- `tests/unit/core/`, using a `SchedulerFixture` from
  `tests/fixtures/`.
- Must be tested against the Virtual Clock's guarantees, not real
  wall-clock time — no `sleep()`-based tests, ever.
- Coverage should include: correct event ordering, priority handling,
  cancellation, and behavior under simultaneous-event ties.
- Structural performance properties (e.g. scheduling N events does
  not exceed expected asymptotic complexity) belong in
  `tests/performance/`, not `tests/unit/`, per Testing Standards §11.

## Simulation Engine

- Primarily `tests/integration/`, since the engine's correctness is
  defined by how core, scheduler, and virtual clock behave together.
- Reproducibility tests are essential here: running the identical
  scenario twice must produce identical traces/outcomes (Vision §3,
  §8) — this should be a standing regression test, not a one-off.
- Scenario-based tests using canned fixture data from
  `tests/fixtures/` (e.g. a small, fixed network-partition scenario)
  give repeatable, human-readable test cases.

## Plugin System

- `tests/unit/plugins/` for individual official plugins once they
  exist, `tests/integration/` for verifying the Plugin Manager
  correctly loads/isolates/unloads plugins without affecting
  `nexus-core` state.
- Because plugins extend the SDK surface, plugin tests should treat
  the SDK as a black box — testing through the same public interface
  a third-party plugin author would use, not internal engine details.

## SDK

- `tests/unit/sdk/`.
- Since the SDK's job (per Vision §9) is exposing a stable contract
  without leaking core implementation detail, SDK tests should assert
  on interface behavior and backward-compatibility guarantees, not on
  `nexus-core` internals.
- Once the SDK stabilizes, consider a dedicated "compatibility" test
  category verifying older SDK usage patterns still compile/run
  against newer core versions.

## CLI

- `tests/integration/`, since the CLI's job is orchestrating core+sdk
  behavior end-to-end.
- Favor testing CLI behavior through its actual argument-parsing and
  output surface (golden-output comparison for command results)
  rather than unit-testing internal CLI functions in isolation.

## Benchmarks

- `nexus-bench` itself (the user-facing product) is validated through
  `tests/integration/` for correctness of its output.
- `tests/benchmarks/` remains for internal micro-benchmarks of
  specific data structures/algorithms used inside `nexus-core`, kept
  separate from the `nexus-bench` product per the module-boundary
  rationale in `docs/testing/TESTING_STANDARDS.md` and Phase 7B's
  directory design.
- Internal benchmarks should record structural performance
  properties over time (regression-style) rather than asserting hard
  wall-clock thresholds, consistent with Testing Standards §11.

---

## General Principle

No subsystem listed above should be implemented without its
corresponding test suite growing alongside it in the same change —
per this phase's guiding principle that testing is foundational, not
appended afterward.