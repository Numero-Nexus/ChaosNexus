// ==========================================
// nexus::core — Identity: Framework Identifiers
// ==========================================
//
// Concrete StrongId<Tag> instantiations for the identifier domains
// needed immediately by the framework. Each Tag is an empty,
// never-instantiated struct that exists solely to make its
// corresponding StrongId a distinct type; add further NodeTag-style
// structs and aliases here as future subsystems need their own ID
// domains, rather than modifying existing ones.

#pragma once

#include "nexus/core/identity/strong_id.hpp"

namespace nexus::core {

namespace detail {

struct NodeTag {};
struct EventTag {};
struct SimulationTag {};

} // namespace detail

/// Identifies a node within a simulation.
using NodeId = StrongId<detail::NodeTag>;

/// Identifies a single simulation event.
using EventId = StrongId<detail::EventTag>;

/// Identifies a simulation run.
using SimulationId = StrongId<detail::SimulationTag>;

} // namespace nexus::core
