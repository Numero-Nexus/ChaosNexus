#include "nexus/core/identity/ids.hpp"
#include "nexus/core/scheduler/policies/round_robin_policy.hpp"

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

TEST(RoundRobinPolicy, ClaimsFairness)
{
    RoundRobinPolicy policy;
    EXPECT_TRUE(policy.claims_fairness());
}

TEST(RoundRobinPolicy, CyclesThroughLanesInRotation)
{
    EventQueue queue;
    const auto a = make_event(1, TimePoint::from_ticks(5));
    const auto b = make_event(2, TimePoint::from_ticks(5));
    queue.push(a);
    queue.push(b);

    RoundRobinPolicy policy;
    policy.assign_lane(a.id(), Lane{0});
    policy.assign_lane(b.id(), Lane{1});

    const auto first = policy.select_next(queue);
    const auto second = policy.select_next(queue);
    EXPECT_NE(first, second);
}

TEST(RoundRobinPolicy, NoIndefiniteStarvationUnderAdversarialPressure)
{
    // Lane A resubmits itself every cycle; lane B has a single
    // pending unit. Verify B is serviced within one full rotation
    // (bound = number of active lanes = 2), not starved indefinitely
    // by A's continuous pressure (FR-018, Master Prompt Section 24).
    EventQueue queue;
    RoundRobinPolicy policy;

    const auto a1 = make_event(1, TimePoint::from_ticks(1));
    const auto b1 = make_event(2, TimePoint::from_ticks(1));
    queue.push(a1);
    queue.push(b1);
    policy.assign_lane(a1.id(), Lane{0});
    policy.assign_lane(b1.id(), Lane{1});

    bool b_serviced = false;
    for (int cycle = 0; cycle < 2 && !queue.empty(); ++cycle) {
        const auto selected = policy.select_next(queue);
        if (selected.id() == b1.id()) {
            b_serviced = true;
        } else {
            const auto a_next = make_event(1, TimePoint::from_ticks(1 + cycle + 1));
            policy.assign_lane(a_next.id(), Lane{0});
            queue.push(a_next);
        }
    }

    EXPECT_TRUE(b_serviced);
}

TEST(RoundRobinPolicy, BoundScalesWithLaneCountUnderSustainedAdversarialPressure)
{
    // N lanes, all under continuous pressure (every lane resubmits
    // immediately after being serviced). Verify every lane is
    // serviced at least once within every window of N consecutive
    // selections, for a long run (Master Prompt Section 24:
    // adversarial in scale and duration, not just in spirit).
    constexpr Lane lane_count = 5;
    constexpr int total_selections = 100;

    EventQueue queue;
    RoundRobinPolicy policy;

    std::uint8_t marker = 1;
    for (Lane lane = 0; lane < lane_count; ++lane) {
        const auto event = make_event(marker++, TimePoint::from_ticks(1));
        queue.push(event);
        policy.assign_lane(event.id(), lane);
    }

    std::vector<Lane> selection_history;
    for (int i = 0; i < total_selections; ++i) {
        const auto selected = policy.select_next(queue);
        const Lane selected_lane = policy.lane_of_public(selected.id());
        selection_history.push_back(selected_lane);

        const auto resubmitted =
            make_event(static_cast<std::uint8_t>(marker++ % 256), TimePoint::from_ticks(1 + i + 1));
        queue.push(resubmitted);
        policy.assign_lane(resubmitted.id(), selected_lane);
    }

    // Sliding-window check: every window of lane_count consecutive
    // selections must contain every lane at least once.
    for (std::size_t start = 0; start + lane_count <= selection_history.size(); ++start) {
        std::vector<bool> seen(lane_count, false);
        for (Lane offset = 0; offset < lane_count; ++offset) {
            seen.at(selection_history.at(start + offset)) = true;
        }
        for (Lane lane = 0; lane < lane_count; ++lane) {
            EXPECT_TRUE(seen.at(lane))
                << "lane " << lane << " missing from window starting at " << start;
        }
    }
}

} // namespace nexus::core::scheduler::policies::test
