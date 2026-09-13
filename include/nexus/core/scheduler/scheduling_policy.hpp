// ==========================================
// nexus::core::scheduler — SchedulingPolicy
// ==========================================
//
// Strategy interface for scheduling-policy selection (FR-017,
// ADR-0006). A policy selects the next event to dispatch from an
// EventQueue, never violating the queue's own (TimePoint,
// insertion_sequence) ordering across distinct eligible timestamps
// (Master Prompt Section 22): a policy may only reorder among events
// already eligible at the same earliest pending timestamp.
//
// Priority and Lane are data attached externally to a schedulable
// unit (by EventId), never hard-coded into Scheduler dispatch logic
// (SDS Section 11).

#pragma once

#include "nexus/core/events/event.hpp"
#include "nexus/core/events/event_queue.hpp"

#include <cstdint>

namespace nexus::core::scheduler {

/// A schedulable unit's policy-interpreted static priority (ADR-0006).
using Priority = std::int32_t;

/// A schedulable unit's round-robin lane identifier (ADR-0006).
using Lane = std::uint32_t;

/// Strategy interface implemented by each built-in policy (ADR-0006:
/// fifo, priority, round_robin).
class SchedulingPolicy {
public:
    SchedulingPolicy() = default;
    virtual ~SchedulingPolicy() = default;
    SchedulingPolicy(const SchedulingPolicy&) = delete;
    auto operator=(const SchedulingPolicy&) -> SchedulingPolicy& = delete;
    SchedulingPolicy(SchedulingPolicy&&) = delete;
    auto operator=(SchedulingPolicy&&) -> SchedulingPolicy& = delete;

    /// Removes and returns the next event to dispatch, per this
    /// policy's selection rule. Precondition: !queue.empty().
    [[nodiscard]] virtual auto select_next(events::EventQueue& queue) -> events::Event = 0;

    /// Returns whether this policy claims a fairness guarantee
    /// (FR-018). Only round_robin returns true (ADR-0006).
    [[nodiscard]] virtual auto claims_fairness() const noexcept -> bool = 0;
};

} // namespace nexus::core::scheduler
