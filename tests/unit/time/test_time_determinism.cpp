// Explicit determinism test for nexus::core::time (Phase 9 — Virtual
// Time, Step 9U). VirtualClock's determinism is already covered by
// Step 9S's VirtualClock.DeterministicGivenIdenticalOperationSequence;
// this file extends the same guarantee to raw Duration/TimePoint
// arithmetic sequences (NFR-007), independent of VirtualClock.

#include "nexus/core/time/duration.hpp"
#include "nexus/core/time/time_point.hpp"

#include <gtest/gtest.h>
#include <vector>

namespace nexus::core::time::test {

namespace {

/// Runs a fixed, representative sequence of TimePoint/Duration
/// operations and returns the final TimePoint plus every intermediate
/// value produced along the way, so determinism can be checked at
/// every step, not just the final result.
auto run_arithmetic_sequence() -> std::vector<TimePoint>
{
    std::vector<TimePoint> trace;

    auto t = TimePoint::from_ticks(0);
    trace.push_back(t);

    t = t + Duration::from_ticks(10);
    trace.push_back(t);

    t = t + Duration::from_ticks(25);
    trace.push_back(t);

    t = t - Duration::from_ticks(5);
    trace.push_back(t);

    const auto elapsed = t - TimePoint::from_ticks(0);
    t = TimePoint::from_ticks(0) + elapsed + Duration::from_ticks(100);
    trace.push_back(t);

    return trace;
}

} // namespace

TEST(TimeDeterminism, IdenticalArithmeticSequenceProducesIdenticalTrace)
{
    const auto first = run_arithmetic_sequence();
    const auto second = run_arithmetic_sequence();
    const auto third = run_arithmetic_sequence();

    ASSERT_EQ(first.size(), second.size());
    ASSERT_EQ(first.size(), third.size());

    for (std::size_t i = 0; i < first.size(); ++i) {
        EXPECT_EQ(first[i], second[i]) << "Mismatch at trace index " << i;
        EXPECT_EQ(first[i], third[i]) << "Mismatch at trace index " << i;
    }
}

TEST(TimeDeterminism, IdenticalArithmeticSequenceProducesIdenticalFinalResult)
{
    const auto first_final = run_arithmetic_sequence().back();
    const auto second_final = run_arithmetic_sequence().back();

    EXPECT_EQ(first_final, second_final);
    EXPECT_EQ(first_final, TimePoint::from_ticks(130));
}

TEST(TimeDeterminism, RepeatedManyTimesRemainsStable)
{
    const auto baseline = run_arithmetic_sequence();

    for (int iteration = 0; iteration < 1000; ++iteration) {
        const auto repeat = run_arithmetic_sequence();
        ASSERT_EQ(baseline, repeat) << "Divergence at iteration " << iteration;
    }
}

} // namespace nexus::core::time::test
