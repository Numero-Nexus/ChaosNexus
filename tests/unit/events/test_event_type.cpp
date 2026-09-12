// Unit tests for nexus::core::events EventType (Phase 10 — Event
// System). Covers construction, equality, and ordering as an opaque
// discriminator. EventType carries no ordering role in Event's own
// deterministic processing order (see test_event.cpp, ADR-0005).

#include "nexus/core/events/event_type.hpp"

#include <gtest/gtest.h>
#include <type_traits>

namespace nexus::core::events::test {

TEST(EventType, DefaultConstructedIsZero)
{
    const EventType t;
    EXPECT_EQ(t.raw(), 0U);
}

TEST(EventType, RawRoundTrips)
{
    const EventType t{42U};
    EXPECT_EQ(t.raw(), 42U);
}

TEST(EventType, EqualityAndOrdering)
{
    const EventType a{5U};
    const EventType b{5U};
    const EventType c{10U};

    EXPECT_EQ(a, b);
    EXPECT_LT(a, c);
    EXPECT_GT(c, a);
    EXPECT_NE(a, c);
}

TEST(EventType, IsTriviallyCopyable)
{
    static_assert(std::is_trivially_copyable_v<EventType>);
}

} // namespace nexus::core::events::test
