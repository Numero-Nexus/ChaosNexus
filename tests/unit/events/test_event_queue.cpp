#include "nexus/core/error/exception.hpp"
#include "nexus/core/events/event.hpp"
#include "nexus/core/events/event_queue.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/time/time_point.hpp"

#include <gtest/gtest.h>
#include <vector>

namespace nexus::core::events::test {

namespace {

[[nodiscard]] auto make_id(core::types::U8 marker) -> EventId
{
    Uuid::ByteArray bytes{};
    bytes[0] = marker;
    return EventId{Uuid{bytes}};
}

} // namespace

TEST(EventQueue, StartsEmpty)
{
    const EventQueue queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0U);
}

TEST(EventQueue, PeekOnEmptyThrows)
{
    const EventQueue queue;
    EXPECT_THROW(queue.peek(), InternalException);
}

TEST(EventQueue, PopOnEmptyThrows)
{
    EventQueue queue;
    EXPECT_THROW((void)queue.pop(), InternalException);
}

TEST(EventQueue, SingleEventPushPop)
{
    EventQueue queue;
    const Event event{make_id(1), time::TimePoint::from_ticks(5), EventType{1U}};

    queue.push(event);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1U);
    EXPECT_EQ(queue.peek(), event);

    const Event popped = queue.pop();
    EXPECT_EQ(popped, event);
    EXPECT_TRUE(queue.empty());
}

TEST(EventQueue, OrdersByTimestampAscending)
{
    EventQueue queue;
    const Event later{make_id(1), time::TimePoint::from_ticks(10), EventType{1U}};
    const Event earlier{make_id(2), time::TimePoint::from_ticks(1), EventType{1U}};
    const Event middle{make_id(3), time::TimePoint::from_ticks(5), EventType{1U}};

    queue.push(later);
    queue.push(earlier);
    queue.push(middle);

    EXPECT_EQ(queue.pop(), earlier);
    EXPECT_EQ(queue.pop(), middle);
    EXPECT_EQ(queue.pop(), later);
}

TEST(EventQueue, SameTimestampStableByInsertionOrder)
{
    EventQueue queue;
    const auto t = time::TimePoint::from_ticks(7);
    const Event first{make_id(1), t, EventType{1U}};
    const Event second{make_id(2), t, EventType{1U}};
    const Event third{make_id(3), t, EventType{1U}};

    queue.push(first);
    queue.push(second);
    queue.push(third);

    EXPECT_EQ(queue.pop(), first);
    EXPECT_EQ(queue.pop(), second);
    EXPECT_EQ(queue.pop(), third);
}

TEST(EventQueue, MixedTimestampsAndTiesOrderCorrectly)
{
    EventQueue queue;
    const auto t0 = time::TimePoint::from_ticks(0);
    const auto t1 = time::TimePoint::from_ticks(1);

    const Event a{make_id(1), t1, EventType{1U}};
    const Event b{make_id(2), t0, EventType{1U}};
    const Event c{make_id(3), t0, EventType{1U}};
    const Event d{make_id(4), t1, EventType{1U}};

    queue.push(a);
    queue.push(b);
    queue.push(c);
    queue.push(d);

    EXPECT_EQ(queue.pop(), b);
    EXPECT_EQ(queue.pop(), c);
    EXPECT_EQ(queue.pop(), a);
    EXPECT_EQ(queue.pop(), d);
}

TEST(EventQueue, DuplicateEventsAreBothRetainedInInsertionOrder)
{
    EventQueue queue;
    const auto t = time::TimePoint::epoch();
    const Event event{make_id(1), t, EventType{1U}};

    queue.push(event);
    queue.push(event);

    EXPECT_EQ(queue.size(), 2U);
    EXPECT_EQ(queue.pop(), event);
    EXPECT_EQ(queue.pop(), event);
    EXPECT_TRUE(queue.empty());
}

TEST(EventQueue, LargeSetExtractsInFullyDeterministicOrder)
{
    EventQueue queue;
    std::vector<Event> expected;

    for (types::I64 tick = 99; tick >= 0; --tick) {
        Event event{make_id(1), time::TimePoint::from_ticks(tick), EventType{1U}};
        queue.push(event);
    }
    for (types::I64 tick = 0; tick <= 99; ++tick) {
        expected.emplace_back(make_id(1), time::TimePoint::from_ticks(tick), EventType{1U});
    }

    EXPECT_EQ(queue.size(), expected.size());
    for (const auto& e : expected) {
        ASSERT_FALSE(queue.empty());
        EXPECT_EQ(queue.pop().timestamp(), e.timestamp());
    }
    EXPECT_TRUE(queue.empty());
}

TEST(EventQueue, RepeatedIdenticalRunsProduceIdenticalExtractionOrder)
{
    const auto build_and_drain = [] {
        EventQueue queue;
        const auto t0 = time::TimePoint::from_ticks(0);
        const auto t1 = time::TimePoint::from_ticks(1);

        queue.push(Event{make_id(1), t1, EventType{1U}});
        queue.push(Event{make_id(2), t0, EventType{1U}});
        queue.push(Event{make_id(3), t0, EventType{1U}});
        queue.push(Event{make_id(4), t1, EventType{1U}});

        std::vector<EventId> order;
        while (!queue.empty()) {
            order.push_back(queue.pop().id());
        }
        return order;
    };

    const auto run1 = build_and_drain();
    const auto run2 = build_and_drain();
    const auto run3 = build_and_drain();

    EXPECT_EQ(run1, run2);
    EXPECT_EQ(run2, run3);
}

} // namespace nexus::core::events::test
