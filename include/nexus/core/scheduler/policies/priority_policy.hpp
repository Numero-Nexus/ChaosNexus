// ==========================================
// nexus::core::scheduler::policies — PriorityPolicy
// ==========================================
//
// Static priority: among events eligible at the earliest pending
// timestamp, selects the one with the highest caller-assigned
// Priority; ties fall back to EventQueue's insertion order
// (ADR-0006). No fairness claim — persistent high-priority pressure
// can starve a lower-priority unit indefinitely; documented, not
// mitigated.
//
// Priority is supplied out-of-band via assign_priority(), since
// Phase 10's Event has no payload (docs/07_EVENT_SYSTEM.md
// Limitations). A unit with no assigned priority defaults to
// Priority{0}.
//
// Known limitation (Step 11J): non-selected events in a same-
// timestamp batch are popped and re-pushed, which assigns them a
// new EventQueue insertion sequence. Their order relative to each
// other is preserved, but their order relative to any event pushed
// by another caller between the original push and this
// select_next() call is not the original arrival order. This does
// not violate NFR-007 (identical call sequences still produce
// identical results) but should be considered before relying on
// exact insertion-sequence semantics for events that pass through
// this policy without being selected.

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/scheduler/scheduling_policy.hpp"

#include <utility>
#include <vector>

namespace nexus::core::scheduler::policies {

class PriorityPolicy final : public SchedulingPolicy {
public:
    auto assign_priority(EventId id, Priority priority) -> void
    {
        priorities_.emplace_back(id, priority);
    }

    [[nodiscard]] auto select_next(events::EventQueue& queue) -> events::Event override
    {
        NEXUS_VERIFY_MSG(!queue.empty(), "PriorityPolicy::select_next: queue is empty");

        // Pull every event sharing the earliest pending timestamp,
        // pick the highest-priority one (arrival order as tie-break),
        // push the rest back unchanged.
        const auto earliest = queue.peek().timestamp();
        std::vector<events::Event> batch;
        while (!queue.empty() && queue.peek().timestamp() == earliest) {
            batch.push_back(queue.pop());
        }

        std::size_t best_index = 0;
        Priority best_priority = priority_of(batch.at(0).id());
        for (std::size_t i = 1; i < batch.size(); ++i) {
            const auto candidate_priority = priority_of(batch.at(i).id());
            if (candidate_priority > best_priority) {
                best_index = i;
                best_priority = candidate_priority;
            }
        }

        events::Event selected = batch.at(best_index);
        for (std::size_t i = 0; i < batch.size(); ++i) {
            if (i != best_index) {
                queue.push(batch.at(i));
            }
        }
        return selected;
    }

    [[nodiscard]] auto claims_fairness() const noexcept -> bool override
    {
        return false;
    }

private:
    [[nodiscard]] static auto priority_of_impl(
        const std::vector<std::pair<EventId, Priority>>& priorities, EventId id) noexcept
        -> Priority
    {
        for (const auto& [entry_id, priority] : priorities) {
            if (entry_id == id) {
                return priority;
            }
        }
        return Priority{0};
    }

    [[nodiscard]] auto priority_of(EventId id) const noexcept -> Priority
    {
        return priority_of_impl(priorities_, id);
    }

    std::vector<std::pair<EventId, Priority>> priorities_;
};

} // namespace nexus::core::scheduler::policies
