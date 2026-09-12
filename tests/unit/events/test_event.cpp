// Unit tests for nexus::core::events Event (Phase 10 — Event
// System). Covers construction, identity/timestamp/type accessors,
// causal-predecessor semantics, equality, and the nil-EventId
// contract violation. Does not test ordering across multiple Events
// -- the (TimePoint, insertion-sequence) total order is a Event
// Queue concern (Steps 10H/10I), not an Event-level concept
// (ADR-0005).

#include "nexus/core/error/exception.hpp"
#include "nexus/core/events/event.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/time/time_point.hpp"

#include <gtest/gtest.h>

namespace nexus::core::events::test {

namespace {

[[nodiscard]] auto make_id(core::types::U8 marker) -> EventId
{
    Uuid::ByteArray bytes{};
    bytes[0] = marker;
    return EventId{Uuid{bytes}};
}

} // namespace

TEST(Event, ConstructsWithRequiredFields)
{
    const auto id = make_id(1);
    const auto timestamp = time::TimePoint::from_ticks(10);
    const EventType type{7U};

    const Event event{id, timestamp, type};

    EXPECT_EQ(event.id(), id);
    EXPECT_EQ(event.timestamp(), timestamp);
    EXPECT_EQ(event.type(), type);
}

TEST(Event, DefaultsToNoCausalPredecessor)
{
    const Event event{make_id(1), time::TimePoint::epoch(), EventType{1U}};

    EXPECT_EQ(event.causal_predecessor(), EventId::nil());
    EXPECT_FALSE(event.has_causal_predecessor());
}

TEST(Event, AcceptsExplicitCausalPredecessor)
{
    const auto predecessor = make_id(2);
    const Event event{make_id(1), time::TimePoint::epoch(), EventType{1U}, predecessor};

    EXPECT_EQ(event.causal_predecessor(), predecessor);
    EXPECT_TRUE(event.has_causal_predecessor());
}

TEST(Event, RejectsNilEventId)
{
    EXPECT_THROW((Event{EventId::nil(), time::TimePoint::epoch(), EventType{1U}}),
                 InternalException);
}

TEST(Event, EqualityComparesAllFields)
{
    const auto id = make_id(1);
    const auto timestamp = time::TimePoint::from_ticks(5);
    const EventType type{3U};
    const auto predecessor = make_id(2);

    const Event a{id, timestamp, type, predecessor};
    const Event b{id, timestamp, type, predecessor};

    EXPECT_EQ(a, b);
}

TEST(Event, InequalityOnDifferingId)
{
    const auto timestamp = time::TimePoint::epoch();
    const EventType type{1U};

    const Event a{make_id(1), timestamp, type};
    const Event b{make_id(2), timestamp, type};

    EXPECT_NE(a, b);
}

TEST(Event, InequalityOnDifferingTimestamp)
{
    const auto id = make_id(1);
    const EventType type{1U};

    const Event a{id, time::TimePoint::from_ticks(1), type};
    const Event b{id, time::TimePoint::from_ticks(2), type};

    EXPECT_NE(a, b);
}

TEST(Event, InequalityOnDifferingType)
{
    const auto id = make_id(1);
    const auto timestamp = time::TimePoint::epoch();

    const Event a{id, timestamp, EventType{1U}};
    const Event b{id, timestamp, EventType{2U}};

    EXPECT_NE(a, b);
}

TEST(Event, InequalityOnDifferingCausalPredecessor)
{
    const auto id = make_id(1);
    const auto timestamp = time::TimePoint::epoch();
    const EventType type{1U};

    const Event a{id, timestamp, type, make_id(2)};
    const Event b{id, timestamp, type, make_id(3)};

    EXPECT_NE(a, b);
}

} // namespace nexus::core::events::test
