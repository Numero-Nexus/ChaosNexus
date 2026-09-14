// ==========================================
// nexus::core::kernel — SimulationComponent
// ==========================================
//
// The minimal abstraction a Simulation Kernel (Phase 12) dispatches
// Events to. No Node/Actor/Process/Component abstraction exists
// anywhere else in the repository (confirmed by repository-wide
// inspection, Step 12-I), so this is introduced fresh, at the
// smallest shape the architecture requires (Master Prompt Section 7):
// something that can receive a single Event and optionally enqueue
// follow-up simulation work. It is deliberately NOT a lifecycle-rich
// actor model (no init/start/stop hooks) -- Phase 12 has no
// requirement driving that, and speculative surface is disallowed
// (Master Prompt Section 11/22).
//
// SimulationComponent does not know its own identity: NodeId
// assignment and Event-to-component routing are ComponentRegistry's
// concern (this file) and, later, the Kernel's dispatch contract
// (Phase 12, Event Dispatch step) -- keeping identity out of the
// component itself avoids a component needing to agree with its
// registry about its own key.
//
// Strategy-interface shape (deleted copy/move, pure virtual,
// noexcept default virtual dtor) mirrors the existing
// nexus::core::scheduler::SchedulingPolicy precedent (Phase 11), the
// closest existing analog in the codebase, for consistency rather
// than inventing a new interface idiom.

#pragma once

#include "nexus/core/events/event.hpp"
#include "nexus/core/events/event_queue.hpp"

namespace nexus::core::kernel {

/// Strategy interface implemented by every simulation-participating
/// component a Kernel can dispatch an Event to.
class SimulationComponent {
public:
    SimulationComponent() = default;
    virtual ~SimulationComponent() = default;
    SimulationComponent(const SimulationComponent&) = delete;
    auto operator=(const SimulationComponent&) -> SimulationComponent& = delete;
    SimulationComponent(SimulationComponent&&) = delete;
    auto operator=(SimulationComponent&&) -> SimulationComponent& = delete;

    /// Executes this component's behaviour for `event`. `queue` is
    /// the same EventQueue instance the owning Kernel drives its
    /// execution loop from: implementations that need to schedule
    /// follow-up simulation work push new Events onto it directly
    /// (Master Prompt Section 20) rather than through any
    /// component-local mechanism. This method does not return a
    /// value: whether follow-up work was scheduled is observable only
    /// through `queue`'s resulting state, matching EventQueue's own
    /// push()-returns-void contract.
    virtual auto on_event(const events::Event& event, events::EventQueue& queue) -> void = 0;
};

} // namespace nexus::core::kernel
