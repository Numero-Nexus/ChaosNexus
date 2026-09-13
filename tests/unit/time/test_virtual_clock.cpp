// Unit tests for nexus::core::time VirtualClock (Phase 9 — Virtual
// Time). Covers initial state, valid/repeated/equal-time advancement,
// invalid backward advancement, and determinism (ADR-0003, ADR-0004).
//
// Phase 11 Step 11F: VirtualClock::advance() now requires a
// VirtualClock::AdvanceKey capability token (ADR-0007). Tests obtain
// one via nexus::core::time::testing::VirtualClockTestAccess, a
// test-only friend that exists solely because
// nexus::core::scheduler::Scheduler (the sole production-sanctioned
// caller) does not yet exist as of this phase.

#include "nexus-test-common/virtual_clock_test_access.hpp"
#include "nexus/core/error/exception.hpp"
#include "nexus/core/time/virtual_clock.hpp"

#include <gtest/gtest.h>

namespace nexus::core::time::test {

namespace {
using nexus::core::time::testing::VirtualClockTestAccess;
} // namespace

TEST(VirtualClock, InitialTimeIsEpoch)
{
    const VirtualClock clock;
    EXPECT_EQ(clock.current(), TimePoint::epoch());
}

TEST(VirtualClock, AdvanceMovesCurrentTimeForward)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(10), VirtualClockTestAccess::make_key());
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(10));
}

TEST(VirtualClock, RepeatedAdvancementAccumulates)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(5), VirtualClockTestAccess::make_key());
    clock.advance(TimePoint::from_ticks(9), VirtualClockTestAccess::make_key());
    clock.advance(TimePoint::from_ticks(20), VirtualClockTestAccess::make_key());

    EXPECT_EQ(clock.current(), TimePoint::from_ticks(20));
}

TEST(VirtualClock, AdvanceToEqualCurrentTimeIsNoOp)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(7), VirtualClockTestAccess::make_key());
    EXPECT_NO_THROW(clock.advance(TimePoint::from_ticks(7), VirtualClockTestAccess::make_key()));
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(7));
}

TEST(VirtualClock, BackwardAdvancementThrows)
{
    VirtualClock clock;
    clock.advance(TimePoint::from_ticks(10), VirtualClockTestAccess::make_key());

    EXPECT_THROW(clock.advance(TimePoint::from_ticks(9), VirtualClockTestAccess::make_key()),
                 InternalException);
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(10));
}

TEST(VirtualClock, DeterministicGivenIdenticalOperationSequence)
{
    const auto run_sequence = []() {
        VirtualClock clock;
        clock.advance(TimePoint::from_ticks(1), VirtualClockTestAccess::make_key());
        clock.advance(TimePoint::from_ticks(4), VirtualClockTestAccess::make_key());
        clock.advance(TimePoint::from_ticks(4), VirtualClockTestAccess::make_key());
        clock.advance(TimePoint::from_ticks(100), VirtualClockTestAccess::make_key());
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
