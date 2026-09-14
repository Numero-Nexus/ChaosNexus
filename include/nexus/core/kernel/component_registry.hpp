// ==========================================
// nexus::core::kernel — ComponentRegistry
// ==========================================
//
// Deterministic ownership and keyed lookup of SimulationComponent
// instances, keyed by the existing NodeId identity domain
// (nexus/core/identity/ids.hpp, "identifies a node within a
// simulation" -- introduced in an earlier phase for exactly this
// purpose, so no new identifier type is added here).
//
// ComponentRegistry owns every component registered with it
// (unique_ptr, RAII) -- nothing else in the architecture has a claim
// to component lifetime (Master Prompt Section 8). It is intentionally
// NOT the Kernel itself: it has no VirtualClock/EventQueue/Scheduler
// dependency and no execution loop, mirroring the precedent set by
// EventQueue not being a Scheduler (event_queue.hpp) -- a registry
// answers only "who is registered" and "look this one up," nothing
// about when or how dispatch happens.
//
// Backed by std::unordered_map<NodeId, ...>. This does not violate
// the project's determinism requirements (NFR-007): the registry is
// only ever used for single-key lookup/insertion by an already-known
// NodeId, never iterated in an order that could affect simulation
// results. If a future phase needs to iterate all registered
// components in a simulation-relevant order, that ordering must be
// established explicitly at that point -- unordered_map's iteration
// order must not be relied upon (Master Prompt Section 12).

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/kernel/component.hpp"

#include <memory>
#include <unordered_map>
#include <utility>

namespace nexus::core::kernel {

/// Owns a set of SimulationComponent instances keyed by NodeId.
class ComponentRegistry {
public:
    ComponentRegistry() = default;

    /// Registers `component` under `id`, transferring ownership to
    /// the registry. `id` must not be the nil NodeId, `component`
    /// must not be null, and `id` must not already be registered --
    /// each is a precondition violation (NEXUS_VERIFY_MSG), not a
    /// recoverable Result<T> error, matching the precedent set by
    /// Event's own nil-id rejection (event.hpp) and EventQueue's
    /// empty-queue preconditions: a caller registering a duplicate or
    /// nil id is a programming error in Kernel setup, not a runtime
    /// condition a simulation scenario can legitimately encounter.
    auto register_component(NodeId id, std::unique_ptr<SimulationComponent> component) -> void
    {
        NEXUS_VERIFY_MSG(!id.is_nil(), "ComponentRegistry::register_component: id must not be nil");
        NEXUS_VERIFY_MSG(component != nullptr,
                         "ComponentRegistry::register_component: component must not be null");
        NEXUS_VERIFY_MSG(!components_.contains(id),
                         "ComponentRegistry::register_component: id already registered");

        components_.emplace(id, std::move(component));
    }

    /// Returns whether a component is registered under `id`.
    [[nodiscard]] auto contains(NodeId id) const noexcept -> bool
    {
        return components_.contains(id);
    }

    /// Returns the number of registered components.
    [[nodiscard]] auto size() const noexcept -> types::Size
    {
        return components_.size();
    }

    /// Returns whether no components are registered.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return components_.empty();
    }

    /// Returns a reference to the component registered under `id`.
    /// Precondition: contains(id). The returned reference is
    /// invalidated only if the registry itself is destroyed --
    /// registration is one-shot (no unregister()), so no other
    /// operation can invalidate it.
    [[nodiscard]] auto get(NodeId id) -> SimulationComponent&
    {
        auto it = components_.find(id);
        NEXUS_VERIFY_MSG(it != components_.end(), "ComponentRegistry::get: id is not registered");
        return *it->second;
    }

private:
    std::unordered_map<NodeId, std::unique_ptr<SimulationComponent>> components_;
};

} // namespace nexus::core::kernel
