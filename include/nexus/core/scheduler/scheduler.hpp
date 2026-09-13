// ==========================================
// nexus::core::scheduler — Scheduler
// ==========================================
//
// Determines execution order and timing (FR-016 through FR-019).
// Owns policy selection and policy-local state only; VirtualClock
// and EventQueue remain non-owned, externally-owned dependencies
// that must outlive this Scheduler (SAD Section 8.3, Master Prompt
// Section 26). Scheduler is the sole production holder of
// VirtualClock::AdvanceKey (ADR-0007) and the sole originator of
// clock-advancement requests (AD-4).
//
// Construction is via the free function make_scheduler(), not a
// Scheduler static member: returning Result<Scheduler> from a member
// declared inside Scheduler's own class body forces concept
// evaluation (move_constructible<Scheduler>) on an incomplete type,
// which MSVC rejects (ADR-0008).
//
// advance_and_select() returns both the selected Event and a
// SchedulingDecision record describing the decision (event, virtual
// time, policy, whether the clock advanced, and a Scheduler-owned
// decision sequence number), satisfying FR-019's reconstructability
// requirement without implementing Observability itself (Master
// Prompt Section 32, Step 11I).

#pragma once

#include "nexus/core/error/result.hpp"
#include "nexus/core/events/event.hpp"
#include "nexus/core/events/event_queue.hpp"
#include "nexus/core/scheduler/scheduling_decision.hpp"
#include "nexus/core/scheduler/scheduling_policy.hpp"
#include "nexus/core/time/virtual_clock.hpp"

#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

namespace nexus::core::scheduler {

class Scheduler {
public:
    /// Constructs directly from a resolved policy and its catalog
    /// name. Prefer make_scheduler(), which validates policy_name
    /// against the ADR-0006 catalog; this constructor is public only
    /// so make_scheduler() can build a Scheduler value to return.
    Scheduler(time::VirtualClock& clock, events::EventQueue& queue,
              std::unique_ptr<SchedulingPolicy> policy, std::string_view policy_name) noexcept
        : clock_{&clock}, queue_{&queue}, policy_{std::move(policy)}, policy_name_{policy_name}
    {}

    /// Selects and removes the next event per the configured policy,
    /// advancing the VirtualClock to its timestamp if needed, and
    /// returns both the selected event and the SchedulingDecision
    /// record describing it (FR-019). Precondition: the bound queue
    /// is non-empty.
    [[nodiscard]] auto advance_and_select() -> std::pair<events::Event, SchedulingDecision>;

    /// Returns whether the configured policy claims a fairness
    /// guarantee (FR-018, ADR-0006).
    [[nodiscard]] auto claims_fairness() const noexcept -> bool
    {
        return policy_->claims_fairness();
    }

private:
    time::VirtualClock* clock_;
    events::EventQueue* queue_;
    std::unique_ptr<SchedulingPolicy> policy_;
    std::string_view policy_name_;
    std::uint64_t decision_sequence_{0};
};

/// Resolves policy_name against the built-in catalog (ADR-0006:
/// "fifo", "priority", "round_robin") and constructs a Scheduler
/// bound to clock/queue (both must outlive it). Returns
/// ErrorCode::UnknownSchedulingPolicy on an unresolvable name
/// (ADR-0008).
[[nodiscard]] auto make_scheduler(time::VirtualClock& clock, events::EventQueue& queue,
                                  std::string_view policy_name) -> Result<Scheduler>;

} // namespace nexus::core::scheduler
