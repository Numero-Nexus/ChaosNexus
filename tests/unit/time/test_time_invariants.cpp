// Property/invariant tests for nexus::core::time (Phase 9 — Virtual
// Time, Step 9T). Covers arithmetic associativity and a generalized
// monotonicity property not already exercised by the Step 9S
// construction/comparison/arithmetic/boundary tests.
//
// Phase 11 Step 11F: VirtualClock::advance() requires a
// VirtualClock::AdvanceKey capability token (ADR-0007), obtained
// here via the test-only VirtualClockTestAccess friend.

#include "nexus-test-common/virtual_clock_test_access.hpp"
#include "nexus/core/time/duration.hpp"
#include "nexus/core/time/time_point.hpp"
#include "nexus/core/time/virtual_clock.hpp"

#include <gtest/gtest.h>

namespace nexus::core::time::test {

namespace {
using nexus::core::time::testing::VirtualClockTestAccess;
} // namespace

TEST(TimeInvariants, DurationAdditionIsAssociative)
{
    const auto d1 = Duration::from_ticks(3);
    const auto d2 = Duration::from_ticks(5);
    const auto d3 = Duration::from_ticks(7);

    EXPECT_EQ((d1 + d2) + d3, d1 + (d2 + d3));
}

TEST(TimeInvariants, TimePointPlusDurationSumIsAssociative)
{
    const auto t = TimePoint::from_ticks(10);
    const auto d1 = Duration::from_ticks(4);
    const auto d2 = Duration::from_ticks(6);

    EXPECT_EQ((t + d1) + d2, t + (d1 + d2));
}

TEST(TimeInvariants, TimePointMinusThenPlusReturnsOriginal)
{
    const auto t = TimePoint::from_ticks(50);
    const auto d = Duration::from_ticks(20);

    EXPECT_EQ((t - d) + d, t);
    EXPECT_EQ((t + d) - d, t);
}

TEST(TimeInvariants, DifferenceThenAddReconstructsLaterPoint)
{
    const auto earlier = TimePoint::from_ticks(10);
    const auto later = TimePoint::from_ticks(35);

    EXPECT_EQ(earlier + (later - earlier), later);
}

class VirtualClockMonotonicityProperty
    : public ::testing::TestWithParam<std::pair<types::I64, types::I64>> {};

TEST_P(VirtualClockMonotonicityProperty, AdvanceToTargetGreaterOrEqualNeverDecreasesCurrent)
{
    const auto [start_ticks, target_ticks] = GetParam();

    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(start_ticks), VirtualClockTestAccess::make_key());
    const auto before = clock.current();

    clock.advance(TimePoint::from_ticks(target_ticks), VirtualClockTestAccess::make_key());
    const auto after = clock.current();

    EXPECT_GE(after, before);
    EXPECT_EQ(after, TimePoint::from_ticks(target_ticks));
}

INSTANTIATE_TEST_SUITE_P(VariousStartAndTargetPairs, VirtualClockMonotonicityProperty,
                         ::testing::Values(std::pair{types::I64{0}, types::I64{0}},
                                           std::pair{types::I64{0}, types::I64{1}},
                                           std::pair{types::I64{5}, types::I64{5}},
                                           std::pair{types::I64{5}, types::I64{6}},
                                           std::pair{types::I64{100}, types::I64{100'000}}));

} // namespace nexus::core::time::test
