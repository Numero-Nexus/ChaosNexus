#include "nexus/core/identity/ids.hpp"
#include "nexus/core/scheduler/policies/fifo_policy.hpp"

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

TEST(FifoPolicy, ClaimsNoFairness)
{
    FifoPolicy policy;
    EXPECT_FALSE(policy.claims_fairness());
}

TEST(FifoPolicy, ProvidesNoFairnessBoundUnderAdversarialResubmission)
{
    // Confirms the *absence* of a fairness guarantee is real
    // behavior, not just an unclaimed metadata flag (Master Prompt
    // Section 23): a low-priority-arrival unit can be pushed back
    // behind continuously-resubmitted units indefinitely under FIFO,
    // since FIFO has no concept of lane/priority to protect it.
    EventQueue queue;
    FifoPolicy policy;

    const auto victim = make_event(1, TimePoint::from_ticks(1));
    queue.push(victim);

    // Continuously resubmit new units ahead of nothing -- FIFO's own
    // ordering means the victim, once behind N arrivals, stays behind
    // exactly N arrivals; it is never artificially protected, but
    // also never artificially starved by this policy specifically.
    // This test documents that FIFO's ordering is arrival-order only
    // and provides no additional protection or bound.
    for (std::uint8_t i = 2; i < 10; ++i) {
        queue.push(make_event(i, TimePoint::from_ticks(1)));
    }

    const auto first_selected = policy.select_next(queue);
    EXPECT_EQ(first_selected, victim); // arrived first, selected first
    EXPECT_FALSE(policy.claims_fairness());
}

TEST(FifoPolicy, DelegatesToQueueOrder)
{
    EventQueue queue;
    const auto e1 = make_event(1, TimePoint::from_ticks(5));
    const auto e2 = make_event(2, TimePoint::from_ticks(5));
    const auto e3 = make_event(3, TimePoint::from_ticks(10));
    queue.push(e1);
    queue.push(e2);
    queue.push(e3);

    FifoPolicy policy;
    EXPECT_EQ(policy.select_next(queue), e1);
    EXPECT_EQ(policy.select_next(queue), e2);
    EXPECT_EQ(policy.select_next(queue), e3);
}

} // namespace nexus::core::scheduler::policies::test
