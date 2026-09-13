// ==========================================
// nexus::core::scheduler::policies — RoundRobinPolicy
// ==========================================
//
// Fair round-robin: schedulable units are grouped by a caller-
// assigned Lane; the policy cycles through lanes in a fixed,
// deterministic rotation, selecting one eligible unit per lane per
// cycle among events eligible at the earliest pending timestamp
// (ADR-0006). Claims no indefinite starvation: every lane with a
// pending eligible unit is guaranteed service within one full
// rotation cycle, bounded by the number of active lanes.
//
// Lane is supplied out-of-band via assign_lane(), for the same
// reason PriorityPolicy takes priority out-of-band: Phase 10's
// Event has no payload.
//
// Known limitation (Step 11J): same re-queue insertion-sequence
// caveat as PriorityPolicy (see priority_policy.hpp) applies here
// for non-selected events in a same-timestamp batch.

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/scheduler/scheduling_policy.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace nexus::core::scheduler::policies {

class RoundRobinPolicy final : public SchedulingPolicy {
public:
    auto assign_lane(EventId id, Lane lane) -> void
    {
        lanes_[id] = lane;
        if (std::ranges::find(rotation_, lane) == rotation_.end()) {
            rotation_.push_back(lane);
        }
    }

    /// Returns the lane assigned to `id`, or Lane{0} if unassigned.
    /// Exposed (read-only) so tests and future observability can
    /// verify fairness properties without duplicating the policy's
    /// internal bookkeeping.
    [[nodiscard]] auto lane_of_public(EventId id) const noexcept -> Lane
    {
        return lane_of(id);
    }

    [[nodiscard]] auto select_next(events::EventQueue& queue) -> events::Event override
    {
        NEXUS_VERIFY_MSG(!queue.empty(), "RoundRobinPolicy::select_next: queue is empty");

        const auto earliest = queue.peek().timestamp();
        std::vector<events::Event> batch;
        while (!queue.empty() && queue.peek().timestamp() == earliest) {
            batch.push_back(queue.pop());
        }

        // Walk the rotation starting at cursor_, find the first lane
        // present in this batch; advance cursor_ past it for the
        // next call. Falls back to arrival order if no lane in the
        // batch has a rotation entry.
        std::size_t chosen_index = 0;
        bool found = false;
        if (!rotation_.empty()) {
            for (std::size_t offset = 0; offset < rotation_.size(); ++offset) {
                const auto candidate_lane = rotation_.at((cursor_ + offset) % rotation_.size());
                for (std::size_t i = 0; i < batch.size(); ++i) {
                    if (lane_of(batch.at(i).id()) == candidate_lane) {
                        chosen_index = i;
                        found = true;
                        cursor_ = (cursor_ + offset + 1) % rotation_.size();
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
        }

        events::Event selected = batch.at(chosen_index);
        for (std::size_t i = 0; i < batch.size(); ++i) {
            if (i != chosen_index) {
                queue.push(batch.at(i));
            }
        }
        return selected;
    }

    [[nodiscard]] auto claims_fairness() const noexcept -> bool override
    {
        return true;
    }

private:
    [[nodiscard]] static auto lane_of_impl(const std::unordered_map<EventId, Lane>& lanes,
                                           EventId id) noexcept -> Lane
    {
        const auto it = lanes.find(id);
        return it != lanes.end() ? it->second : Lane{0};
    }

    [[nodiscard]] auto lane_of(EventId id) const noexcept -> Lane
    {
        return lane_of_impl(lanes_, id);
    }

    std::unordered_map<EventId, Lane> lanes_;
    std::vector<Lane> rotation_;
    std::size_t cursor_{0};
};

} // namespace nexus::core::scheduler::policies
