// ==========================================
// nexus::core::scheduler::policies — FifoPolicy
// ==========================================
//
// FIFO: delegates entirely to EventQueue's own (timestamp,
// insertion_sequence) order (ADR-0006). No fairness claim: strict
// submission-order service is not the same as a bounded-starvation
// guarantee under adversarial scenario construction.

#pragma once

#include "nexus/core/scheduler/scheduling_policy.hpp"

namespace nexus::core::scheduler::policies {

class FifoPolicy final : public SchedulingPolicy {
public:
    [[nodiscard]] auto select_next(events::EventQueue& queue) -> events::Event override
    {
        return queue.pop();
    }

    [[nodiscard]] auto claims_fairness() const noexcept -> bool override
    {
        return false;
    }
};

} // namespace nexus::core::scheduler::policies
