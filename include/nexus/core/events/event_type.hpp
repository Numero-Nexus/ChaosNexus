// ==========================================
// nexus::core::events — EventType
// ==========================================
//
// An opaque, extensible discriminator for what kind of event an
// Event represents. Deliberately not a fixed enum: the SRS (FR-009)
// requires plugin-defined event types to participate identically to
// built-in ones, but no Scheduler, Messaging, or Plugin Manager
// exists yet to define concrete kinds (Phase 11+). Hard-coding a
// closed enum here would need to be revisited the moment any of
// those phases lands, so EventType instead wraps a raw numeric
// discriminator whose concrete value catalog is left to the module
// that owns event semantics for a given kind (built-in or plugin).
//
// EventType participates in equality/ordering only for its own sake
// (e.g. future dispatch routing); it is explicitly NOT part of the
// deterministic event ordering relation (see event.hpp, ADR-0005).

#pragma once

#include "nexus/core/types/types.hpp"

#include <compare>

namespace nexus::core::events {

/// Opaque discriminator for the kind of an Event. Value semantics,
/// trivially copyable, totally ordered only so it can be used as a
/// key/sorted where needed (e.g. dispatch tables) -- this ordering
/// has no bearing on Event's own deterministic processing order.
class EventType {
public:
    constexpr EventType() noexcept = default;

    explicit constexpr EventType(types::U32 raw) noexcept : raw_{raw} {}

    [[nodiscard]] constexpr auto raw() const noexcept -> types::U32
    {
        return raw_;
    }

    [[nodiscard]] friend constexpr auto operator==(const EventType&, const EventType&) noexcept
        -> bool = default;

    [[nodiscard]] friend constexpr auto operator<=>(const EventType&, const EventType&) noexcept
        -> std::strong_ordering = default;

private:
    types::U32 raw_{0};
};

} // namespace nexus::core::events
