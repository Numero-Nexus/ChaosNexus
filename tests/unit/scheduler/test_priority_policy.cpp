#include "nexus/core/identity/ids.hpp"
#include "nexus/core/scheduler/policies/priority_policy.hpp"

#include <gtest/gtest.h>

namespace nexus::core::scheduler::policies::test {

namespace {

using nexus::core::EventId;
using nexus::core::Uuid;
using nexus::core::events::Event;
using nexus::core::events::EventQueue;
using nexus::core::events::EventType;
using nexus::core::time::TimePoint;

[[nodiscard]] auto make_id(core::types::U8 marker) -> EventId
{
    Uuid::ByteArray bytes{};
    bytes[0] = marker;
    return EventId{Uuid{bytes}};
}

[[nodiscard]] auto make_event(core::types::U8 marker, TimePoint t) -> Event
{
    return Event{make_id(marker), t, EventType{1U}};
}

} // namespace

TEST(PriorityPolicy, ClaimsNoFairness)
{
    PriorityPolicy policy;
    EXPECT_FALSE(policy.claims_fairness());
}

TEST(PriorityPolicy, SelectsHighestPriorityAmongSameTimestamp)
{
    EventQueue queue;
    const auto low = make_event(1, TimePoint::from_ticks(5));
    const auto high = make_event(2, TimePoint::from_ticks(5));
    queue.push(low);
    queue.push(high);

    PriorityPolicy policy;
    policy.assign_priority(low.id(), Priority{1});
    policy.assign_priority(high.id(), Priority{10});

    EXPECT_EQ(policy.select_next(queue), high);
    EXPECT_EQ(policy.select_next(queue), low);
}

TEST(PriorityPolicy, UnassignedUnitsDefaultToZeroPriority)
{
    EventQueue queue;
    const auto unassigned = make_event(1, TimePoint::from_ticks(5));
    const auto negative = make_event(2, TimePoint::from_ticks(5));
    queue.push(unassigned);
    queue.push(negative);

    PriorityPolicy policy;
    policy.assign_priority(negative.id(), Priority{-1});

    EXPECT_EQ(policy.select_next(queue), unassigned);
    EXPECT_EQ(policy.select_next(queue), negative);
}

TEST(PriorityPolicy, NeverSelectsLaterTimestampOverEarlierOne)
{
    EventQueue queue;
    const auto earlier = make_event(1, TimePoint::from_ticks(5));
    const auto later_high_priority = make_event(2, TimePoint::from_ticks(10));
    queue.push(earlier);
    queue.push(later_high_priority);

    PriorityPolicy policy;
    policy.assign_priority(later_high_priority.id(), Priority{100});

    EXPECT_EQ(policy.select_next(queue), earlier);
}

} // namespace nexus::core::scheduler::policies::test
