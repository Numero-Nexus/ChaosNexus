// Unit tests for nexus::core::time Duration (Phase 9 — Virtual
// Time). Covers construction, comparison, arithmetic, and overflow
// contract violations (ADR-0001, ADR-0004).

#include "nexus/core/error/exception.hpp"
#include "nexus/core/time/duration.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <type_traits>

namespace nexus::core::time::test {

TEST(Duration, DefaultConstructedIsZero)
{
    const Duration d;
    EXPECT_EQ(d.ticks(), 0);
    EXPECT_EQ(d, Duration::zero());
}

TEST(Duration, FromTicksRoundTripsPositiveAndNegative)
{
    EXPECT_EQ(Duration::from_ticks(42).ticks(), 42);
    EXPECT_EQ(Duration::from_ticks(-42).ticks(), -42);
    EXPECT_EQ(Duration::from_ticks(0).ticks(), 0);
}

TEST(Duration, FromTicksRejectsI64Min)
{
    EXPECT_THROW(Duration::from_ticks(std::numeric_limits<types::I64>::min()), InternalException);
}

TEST(Duration, EqualityAndOrdering)
{
    const auto a = Duration::from_ticks(5);
    const auto b = Duration::from_ticks(5);
    const auto c = Duration::from_ticks(10);

    EXPECT_EQ(a, b);
    EXPECT_LT(a, c);
    EXPECT_GT(c, a);
    EXPECT_LE(a, b);
    EXPECT_GE(c, a);
}

TEST(Duration, AdditionAndSubtraction)
{
    const auto a = Duration::from_ticks(5);
    const auto b = Duration::from_ticks(3);

    EXPECT_EQ((a + b).ticks(), 8);
    EXPECT_EQ((a - b).ticks(), 2);
    EXPECT_EQ((b - a).ticks(), -2);
}

TEST(Duration, AdditionIdentity)
{
    const auto a = Duration::from_ticks(7);
    EXPECT_EQ(a + Duration::zero(), a);
    EXPECT_EQ(a - Duration::zero(), a);
}

TEST(Duration, AdditionOverflowThrows)
{
    const auto max = Duration::from_ticks(std::numeric_limits<types::I64>::max());
    EXPECT_THROW(max + Duration::from_ticks(1), InternalException);
}

TEST(Duration, SubtractionUnderflowThrows)
{
    const auto near_min = Duration::from_ticks(std::numeric_limits<types::I64>::min() + 5);
    EXPECT_THROW(near_min - Duration::from_ticks(10), InternalException);
}

TEST(Duration, IsTriviallyCopyable)
{
    static_assert(std::is_trivially_copyable_v<Duration>);
    static_assert(sizeof(Duration) == sizeof(types::I64));
}

} // namespace nexus::core::time::test
