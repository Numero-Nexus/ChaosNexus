#include "nexus/core/identity/ids.hpp"
#include "nexus/core/scheduler/scheduler.hpp"

#include <gtest/gtest.h>

namespace nexus::core::scheduler::test {

namespace {

using nexus::core::EventId;
using nexus::core::Uuid;
using nexus::core::events::Event;
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

TEST(Scheduler, RejectsUnknownPolicyName)
{
    time::VirtualClock clock;
    events::EventQueue queue;

    auto result = make_scheduler(clock, queue, "not_a_real_policy");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code(), ErrorCode::UnknownSchedulingPolicy);
}

TEST(Scheduler, AcceptsEachBuiltinPolicyName)
{
    time::VirtualClock clock;
    events::EventQueue queue;

    for (const auto* name : {"fifo", "priority", "round_robin"}) {
        auto result = make_scheduler(clock, queue, name);
        EXPECT_TRUE(result.has_value()) << "policy: " << name;
    }
}

TEST(Scheduler, AdvanceAndSelectAdvancesClockToSelectedTimestamp)
{
    time::VirtualClock clock;
    events::EventQueue queue;
    queue.push(make_event(1, TimePoint::from_ticks(10)));

    auto result = make_scheduler(clock, queue, "fifo");
    ASSERT_TRUE(result.has_value());
    Scheduler scheduler = std::move(result).value();

    const auto [selected, decision] = scheduler.advance_and_select();
    EXPECT_EQ(selected.timestamp(), TimePoint::from_ticks(10));
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(10));
    EXPECT_TRUE(decision.clock_advanced);
    EXPECT_EQ(decision.scheduled_timestamp, TimePoint::from_ticks(10));
    EXPECT_EQ(decision.event_id, selected.id());
    EXPECT_EQ(decision.policy_name, "fifo");
    EXPECT_EQ(decision.sequence, 0U);
}

TEST(Scheduler, AdvanceAndSelectIsNoOpWhenTimestampNotAfterCurrent)
{
    time::VirtualClock clock;
    events::EventQueue queue;
    queue.push(make_event(1, TimePoint::epoch()));

    auto result = make_scheduler(clock, queue, "fifo");
    ASSERT_TRUE(result.has_value());
    Scheduler scheduler = std::move(result).value();

    const auto [selected, decision] = scheduler.advance_and_select();
    EXPECT_EQ(clock.current(), TimePoint::epoch());
    EXPECT_FALSE(decision.clock_advanced);
}

TEST(Scheduler, DecisionSequenceIncrementsAcrossCalls)
{
    time::VirtualClock clock;
    events::EventQueue queue;
    queue.push(make_event(1, TimePoint::from_ticks(1)));
    queue.push(make_event(2, TimePoint::from_ticks(2)));

    auto result = make_scheduler(clock, queue, "fifo");
    ASSERT_TRUE(result.has_value());
    Scheduler scheduler = std::move(result).value();

    const auto [event1, decision1] = scheduler.advance_and_select();
    const auto [event2, decision2] = scheduler.advance_and_select();

    EXPECT_EQ(decision1.sequence, 0U);
    EXPECT_EQ(decision2.sequence, 1U);
}

TEST(Scheduler, ClockAdvancesMonotonicallyAcrossMultipleDistinctTimestamps)
{
    time::VirtualClock clock;
    events::EventQueue queue;
    queue.push(make_event(1, TimePoint::from_ticks(3)));
    queue.push(make_event(2, TimePoint::from_ticks(7)));
    queue.push(make_event(3, TimePoint::from_ticks(15)));

    auto result = make_scheduler(clock, queue, "fifo");
    ASSERT_TRUE(result.has_value());
    Scheduler scheduler = std::move(result).value();

    auto before = clock.current();
    for (int i = 0; i < 3; ++i) {
        const auto [selected, decision] = scheduler.advance_and_select();
        EXPECT_GE(clock.current(), before);
        EXPECT_TRUE(decision.clock_advanced);
        before = clock.current();
    }
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(15));
}

TEST(Scheduler, NeverConstructsClockOutsideVirtualClock)
{
    // Confirms Scheduler carries no independent notion of time: the
    // only way its decisions reflect elapsed virtual time is by
    // reading clock.current() through the bound VirtualClock&.
    time::VirtualClock clock;
    events::EventQueue queue;
    queue.push(make_event(1, TimePoint::from_ticks(42)));

    auto result = make_scheduler(clock, queue, "fifo");
    ASSERT_TRUE(result.has_value());
    Scheduler scheduler = std::move(result).value();

    EXPECT_EQ(clock.current(), TimePoint::epoch());
    scheduler.advance_and_select();
    EXPECT_EQ(clock.current(), TimePoint::from_ticks(42));
}

namespace {

/// Runs a fixed sequence of pushes/selects against a fresh Scheduler
/// bound to fresh VirtualClock/EventQueue instances, and returns
/// every observable output in order: (event id, timestamp,
/// clock_advanced, sequence) per decision, plus the final clock
/// value. Used to verify NFR-007 across independent runs (Master
/// Prompt Section 39: "same inputs = same scheduling decisions").
struct RunTrace {
    struct Step {
        EventId id;
        TimePoint timestamp;
        bool clock_advanced;
        std::uint64_t sequence;

        auto operator==(const Step&) const -> bool = default;
    };

    std::vector<Step> steps;
    TimePoint final_clock;

    auto operator==(const RunTrace&) const -> bool = default;
};

[[nodiscard]] auto run_fixed_scenario(std::string_view policy_name) -> RunTrace
{
    time::VirtualClock clock;
    events::EventQueue queue;

    queue.push(make_event(1, TimePoint::from_ticks(5)));
    queue.push(make_event(2, TimePoint::from_ticks(5)));
    queue.push(make_event(3, TimePoint::from_ticks(12)));
    queue.push(make_event(4, TimePoint::from_ticks(12)));
    queue.push(make_event(5, TimePoint::from_ticks(20)));

    auto result = make_scheduler(clock, queue, policy_name);
    if (!result.has_value()) {
        return RunTrace{};
    }
    Scheduler scheduler = std::move(result).value();

    RunTrace trace;
    while (!queue.empty()) {
        const auto [selected, decision] = scheduler.advance_and_select();
        trace.steps.push_back({.id = selected.id(),
                               .timestamp = decision.scheduled_timestamp,
                               .clock_advanced = decision.clock_advanced,
                               .sequence = decision.sequence});
    }
    trace.final_clock = clock.current();
    return trace;
}

} // namespace

TEST(Scheduler, RepeatedIdenticalRunsProduceIdenticalTraceForFifo)
{
    const auto first = run_fixed_scenario("fifo");
    const auto second = run_fixed_scenario("fifo");
    const auto third = run_fixed_scenario("fifo");

    EXPECT_EQ(first, second);
    EXPECT_EQ(second, third);
    EXPECT_EQ(first.final_clock, TimePoint::from_ticks(20));
}

TEST(Scheduler, RepeatedIdenticalRunsProduceIdenticalTraceForPriority)
{
    const auto first = run_fixed_scenario("priority");
    const auto second = run_fixed_scenario("priority");

    EXPECT_EQ(first, second);
}

TEST(Scheduler, RepeatedIdenticalRunsProduceIdenticalTraceForRoundRobin)
{
    const auto first = run_fixed_scenario("round_robin");
    const auto second = run_fixed_scenario("round_robin");

    EXPECT_EQ(first, second);
}

} // namespace nexus::core::scheduler::test
