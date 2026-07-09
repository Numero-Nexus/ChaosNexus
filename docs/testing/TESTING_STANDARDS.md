# ChaosNexus — Testing Standards

## 1. Purpose

This document defines project-wide testing conventions for ChaosNexus.
It applies to all current and future test suites under `tests/`.
Established during Phase 7B (Testing Infrastructure).

---

## 2. Naming Conventions

- **Test executables**: `<module>-tests` (e.g. `nexus-core-tests`,
  `nexus-sdk-tests`). One executable per `tests/unit/<module>/`
  directory.
- **Test files**: `<unit_under_test>_test.cpp` (e.g.
  `bootstrap_test.cpp`, `scheduler_test.cpp`). One file per logical
  unit under test, not one file per class method.
- **Test suites (GoogleTest `TEST`/`TEST_F` first argument)**:
  `PascalCase`, named after the unit under test plus `Test` suffix
  where it aids clarity (e.g. `SchedulerTest`, `BootstrapSmokeTest`).
- **Individual test cases (second argument)**: `PascalCase`,
  describing the behavior being verified, not the implementation
  (e.g. `ReturnsTrue`, `RejectsNegativeDuration`, not `Test1` or
  `CheckInternalFlag`).
- **Fixture classes** (`tests/fixtures/`): `<Concept>Fixture` (e.g.
  `SchedulerFixture`, `VirtualClockFixture`).

---

## 3. Test Layout

- Tests are organized by module first (`unit/core`, `unit/sdk`, ...),
  mirroring the top-level `nexus-*` directory structure established
  in Phase 7A. A contributor should always be able to locate a
  module's tests without searching.
- Within a module's test directory, one `.cpp` file per unit under
  test (a class, a free-function group, or a tightly related set of
  behaviors) — not one giant file per module.
- Cross-module behavior belongs in `tests/integration/`, never in
  `tests/unit/`. A unit test that requires more than one `nexus-*`
  module to construct its scenario is a signal it belongs in
  `integration/` instead.

---

## 4. Fixture Organization

- Prefer plain `TEST()` for stateless, trivial assertions (as in the
  Phase 7B smoke test).
- Use `TEST_F()` with a fixture class once setup/teardown logic is
  shared across more than one test case.
- Shared fixtures used by more than one module's test suite belong in
  `tests/fixtures/`, not duplicated per module.
- Fixtures must not depend on external state (filesystem, network,
  wall-clock time, environment variables) — see Determinism, below.

---

## 5. Assertions

- Prefer `EXPECT_*` over `ASSERT_*` by default, since `EXPECT_*`
  allows a test to report all failures in one run rather than
  aborting at the first one.
- Use `ASSERT_*` only when a failed precondition would make
  subsequent checks in the same test meaningless or unsafe (e.g. a
  null pointer that would be dereferenced next).
- Avoid testing implementation details (private state, internal call
  counts) in favor of observable behavior through public interfaces.

---

## 6. Failure Reporting

- Test names and assertion messages must make a failure
  self-explanatory from CTest/CI output alone, without needing to
  open the source file.
- Avoid bare `EXPECT_TRUE(condition)` where a more specific matcher
  exists (`EXPECT_EQ`, `EXPECT_THAT`, etc.) — specific matchers
  produce more informative failure output (expected vs. actual
  values) than a boolean collapse.

---

## 7. AAA Pattern (Arrange-Act-Assert)

Every test should have visually distinct Arrange, Act, and Assert
sections, in that order, ideally separated by a blank line:

```cpp
TEST(SchedulerTest, ProcessesEventsInTimeOrder) {
    // Arrange
    Scheduler scheduler;
    scheduler.schedule(Event{...});

    // Act
    const auto result = scheduler.run_until(some_time);

    // Assert
    EXPECT_EQ(result.events_processed, 1);
}
```

A test that doesn't fit this shape is usually testing too much at
once and should be split.

---

## 8. Deterministic Testing

- Tests must never depend on wall-clock time, real thread scheduling,
  random number generators without a fixed seed, or filesystem/network
  state outside the test's own control.
- ChaosNexus's core premise (per the Vision) is deterministic
  simulation — this standard applies doubly to its own test suite.
  Any test whose pass/fail outcome could vary between two runs on the
  same code is a defect in the test, not acceptable flakiness.

---

## 9. Isolation

- Each test must be independently runnable and independently
  meaningful — no test may depend on state left behind by a previous
  test (no shared mutable globals, no execution-order assumptions).
- CTest and GoogleTest may execute tests in parallel or in any order;
  tests that assume otherwise are defects.

---

## 10. Repeatability

- Running the same test suite twice, on the same code, must produce
  identical results. This follows directly from Determinism (§8) and
  Isolation (§9) and is the same property ChaosNexus's core engine is
  ultimately meant to guarantee for simulations themselves.

---

## 11. Performance Expectations

- Unit tests (`tests/unit/`) must run fast — individual tests should
  complete in milliseconds, not seconds. Slow setup belongs in
  `tests/integration/` or `tests/performance/`, not `tests/unit/`.
- The full unit test suite should remain fast enough to run on every
  local build without friction. If it stops being fast, that is a
  signal to split slow tests out into `integration/` or
  `performance/`, not to reduce test coverage.
- `tests/performance/` tests may take longer but must still be
  deterministic (§8) — asserting on relative/structural performance
  properties (e.g. "does not exceed O(n log n) growth") rather than
  absolute wall-clock thresholds, which vary by machine and are not
  reproducible across environments.