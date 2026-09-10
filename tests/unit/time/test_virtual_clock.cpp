// Unit tests for nexus::core::time VirtualClock (Phase 9 — Virtual
// Time). Covers initial state, valid/repeated/equal-time advancement,
// invalid backward advancement, and determinism (ADR-0003, ADR-0004).

#include "nexus/core/error/exception.hpp"
#include "nexus/core/time/virtual_clock.hpp"

#include <gtest/gtest.h>

namespace nexus::core::time::test {

TEST(VirtualClock, InitialTimeIsEpoch)
{
    const VirtualClock clock;
    EXPECT_EQ(clock.current(), TimePoint::epoch());
}

TEST(VirtualClock, AdvanceMovesCurrentTimeForward)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(10));
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(10));
}

TEST(VirtualClock, RepeatedAdvancementAccumulates)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(5));
    clock.advance(TimePoint::from_ticks(9));
    clock.advance(TimePoint::from_ticks(20));

    EXPECT_EQ(clock.current(), TimePoint::from_ticks(20));
}

TEST(VirtualClock, AdvanceToEqualCurrentTimeIsNoOp)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(7));
    EXPECT_NO_THROW(clock.advance(TimePoint::from_ticks(7)));
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(7));
}

TEST(VirtualClock, BackwardAdvancementThrows)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(10));

    EXPECT_THROW(clock.advance(TimePoint::from_ticks(9)), InternalException);
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(10));
}

TEST(VirtualClock, DeterministicGivenIdenticalOperationSequence)
{
    const auto run_sequence = []() {
        VirtualClock clock;
        clock.advance(TimePoint::from_ticks(1));
        clock.advance(TimePoint::from_ticks(4));
        clock.advance(TimePoint::from_ticks(4));
        clock.advance(TimePoint::from_ticks(100));
        return clock.current();
    };

    const auto first = run_sequence();
    const auto second = run_sequence();
    const auto third = run_sequence();

    EXPECT_EQ(first, second);
    EXPECT_EQ(second, third);
    EXPECT_EQ(first, TimePoint::from_ticks(100));
}

} // namespace nexus::core::time::test
