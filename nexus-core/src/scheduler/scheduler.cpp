#include "nexus/core/scheduler/scheduler.hpp"

#include "nexus/core/error/helpers.hpp"
#include "nexus/core/scheduler/policies/fifo_policy.hpp"
#include "nexus/core/scheduler/policies/priority_policy.hpp"
#include "nexus/core/scheduler/policies/round_robin_policy.hpp"

#include <utility>

namespace nexus::core::scheduler {

auto make_scheduler(time::VirtualClock& clock, events::EventQueue& queue,
                    std::string_view policy_name) -> Result<Scheduler>
{
    std::unique_ptr<SchedulingPolicy> policy;
    if (policy_name == "fifo") {
        policy = std::unique_ptr<SchedulingPolicy>{std::make_unique<policies::FifoPolicy>()};
    } else if (policy_name == "priority") {
        policy = std::unique_ptr<SchedulingPolicy>{std::make_unique<policies::PriorityPolicy>()};
    } else if (policy_name == "round_robin") {
        policy = std::unique_ptr<SchedulingPolicy>{std::make_unique<policies::RoundRobinPolicy>()};
    } else {
        return failure<Scheduler>(ErrorCode::UnknownSchedulingPolicy,
                                  "make_scheduler: unknown scheduling policy");
    }

    return success(Scheduler{clock, queue, std::move(policy), policy_name});
}

auto Scheduler::advance_and_select() -> std::pair<events::Event, SchedulingDecision>
{
    events::Event selected = policy_->select_next(*queue_);

    const bool needs_advance = selected.timestamp() > clock_->current();
    if (needs_advance) {
        clock_->advance(selected.timestamp(), time::VirtualClock::AdvanceKey{});
    }

    const SchedulingDecision decision{
        .event_id = selected.id(),
        .scheduled_timestamp = selected.timestamp(),
        .policy_name = policy_name_,
        .clock_advanced = needs_advance,
        .sequence = decision_sequence_++,
    };

    return {selected, decision};
}

} // namespace nexus::core::scheduler
