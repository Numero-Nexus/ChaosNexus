// Unit tests for nexus::core::time TimePoint (Phase 9 — Virtual
// Time). Covers construction, comparison, arithmetic with Duration,
// and boundary/contract-violation behavior (ADR-0001, ADR-0004).

#include "nexus/core/error/exception.hpp"
#include "nexus/core/time/duration.hpp"
#include "nexus/core/time/time_point.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <type_traits>

namespace nexus::core::time::test {

TEST(TimePoint, DefaultConstructedIsEpoch)
{
    const TimePoint t;
    EXPECT_EQ(t, TimePoint::epoch());
    EXPECT_EQ(t.ticks(), 0);
}

TEST(TimePoint, FromTicksRoundTrips)
{
    EXPECT_EQ(TimePoint::from_ticks(100).ticks(), 100);
    EXPECT_EQ(TimePoint::from_ticks(0), TimePoint::epoch());
}

TEST(TimePoint, FromTicksRejectsNegative)
{
    EXPECT_THROW(TimePoint::from_ticks(-1), InternalException);
}

TEST(TimePoint, EqualityAndOrdering)
{
    const auto a = TimePoint::from_ticks(5);
    const auto b = TimePoint::from_ticks(5);
    const auto c = TimePoint::from_ticks(10);

    EXPECT_EQ(a, b);
    EXPECT_LT(a, c);
    EXPECT_GT(c, a);
}

TEST(TimePoint, AddDurationAdvancesTime)
{
    const auto t = TimePoint::from_ticks(10);
    const auto d = Duration::from_ticks(5);

    EXPECT_EQ((t + d).ticks(), 15);
}

TEST(TimePoint, SubtractDurationMovesBackward)
{
    const auto t = TimePoint::from_ticks(10);
    const auto d = Duration::from_ticks(5);

    EXPECT_EQ((t - d).ticks(), 5);
}

TEST(TimePoint, AddZeroDurationIsIdentity)
{
    const auto t = TimePoint::from_ticks(42);
    EXPECT_EQ(t + Duration::zero(), t);
    EXPECT_EQ(t - Duration::zero(), t);
}

TEST(TimePoint, DifferenceOfTwoTimePointsYieldsDuration)
{
    const auto later = TimePoint::from_ticks(20);
    const auto earlier = TimePoint::from_ticks(8);

    EXPECT_EQ((later - earlier).ticks(), 12);
    EXPECT_EQ((earlier - later).ticks(), -12);
}

TEST(TimePoint, DifferenceOfIdenticalPointsIsZero)
{
    const auto t = TimePoint::from_ticks(30);
    EXPECT_EQ(t - t, Duration::zero());
}

TEST(TimePoint, SubtractingDurationBelowZeroThrows)
{
    const auto t = TimePoint::from_ticks(5);
    EXPECT_THROW(t - Duration::from_ticks(10), InternalException);
}

TEST(TimePoint, AdditionOverflowThrows)
{
    const auto max = TimePoint::from_ticks(std::numeric_limits<types::I64>::max());
    EXPECT_THROW(max + Duration::from_ticks(1), InternalException);
}

TEST(TimePoint, IsTriviallyCopyable)
{
    static_assert(std::is_trivially_copyable_v<TimePoint>);
    static_assert(sizeof(TimePoint) == sizeof(types::I64));
}

} // namespace nexus::core::time::test
