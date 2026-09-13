// ==========================================
// nexus::core::scheduler — SchedulingDecision
// ==========================================
//
// Passive data record of a single scheduling decision, sufficient
// for future reconstruction from trace output (FR-019). This is a
// data contract only — Phase 11 does not implement Observability,
// logging, or serialization (Master Prompt Section 32). A later
// Observability subsystem may consume SchedulingDecision values;
// Scheduler does not depend on how or whether they are recorded.

#pragma once

#include "nexus/core/identity/ids.hpp"
#include "nexus/core/time/time_point.hpp"

#include <cstdint>
#include <string_view>

namespace nexus::core::scheduler {

/// Records what was selected, when, by which policy, and whether the
/// decision advanced virtual time. Scheduler-owned sequence number
/// counts decisions made, distinct from EventQueue's own insertion
/// sequence (that ordering authority is not duplicated here).
struct SchedulingDecision {
    EventId event_id;
    time::TimePoint scheduled_timestamp;
    std::string_view policy_name;
    bool clock_advanced;
    std::uint64_t sequence;
};

} // namespace nexus::core::scheduler
