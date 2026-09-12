// ==========================================
// nexus::core::events — Event
// ==========================================
//
// The uniform envelope every simulation event carries, regardless of
// origin (SDS Section 9): identity, an absolute virtual timestamp,
// a type discriminator, and an optional causal-predecessor reference.
//
// Design contract (Phase 10, Steps 10D/10E):
//   - Identity reuses the existing EventId = StrongId<EventTag>
//     (nexus/core/identity/ids.hpp) established in Phase 8. No new
//     identity type is introduced.
//   - Timestamp reuses nexus::core::time::TimePoint (Phase 9) as the
//     sole ordering-relevant field on Event itself.
//   - Causal predecessor is an EventId; EventId::nil() denotes "no
//     predecessor" (e.g. a scenario-start/root event), avoiding a
//     second sentinel/optional vocabulary for what is already a
//     nil-representable strong ID domain.
//   - All fields are immutable after construction (Master Prompt
//     Section 14): a queued Event cannot mutate its ordering key.
//   - Value semantics: Event is a plain, copyable/movable data type,
//     not an engine-owned polymorphic object (SDS Section 3/8).
//
// Event deliberately does NOT define operator<=>. The full
// deterministic ordering relation is (TimePoint, insertion sequence)
// per ADR-0005; the insertion sequence is assigned by whatever
// structure accepts an Event for ordered processing (the Event
// Queue, Phase 10 Steps 10H/10I), not by Event itself -- an Event
// has no notion of "when it was submitted relative to others" prior
// to submission. Exposing only timestamp() here, rather than a
// (correct-looking but incomplete) operator<=>, avoids silently
// inviting callers to sort Events by timestamp alone and get an
// under-specified, non-deterministic same-timestamp order.
//
// EventType is explicitly excluded from ordering and equality has no
// bearing on it either way; payload representation is not part of
// Phase 10's Event Model Contract (Step 10D) since no consumer
// (Scheduler, Messaging) exists yet to justify a shape -- adding one
// speculatively would violate Master Prompt Section 12.

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/events/event_type.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/time/time_point.hpp"

namespace nexus::core::events {

/// The uniform event envelope. Immutable after construction.
class Event {
public:
    /// Constructs an Event. `id` must not be the nil EventId -- every
    /// submitted event must carry a genuine identity (Master Prompt
    /// Section 8/14). `causal_predecessor` defaults to EventId::nil(),
    /// meaning "no causal predecessor" (a root/originating event).
    Event(EventId id, time::TimePoint timestamp, EventType type,
          EventId causal_predecessor = EventId::nil())
        : id_{id}, timestamp_{timestamp}, type_{type}, causal_predecessor_{causal_predecessor}
    {
        NEXUS_VERIFY_MSG(!id_.is_nil(), "Event: id must not be the nil EventId");
    }

    [[nodiscard]] constexpr auto id() const noexcept -> EventId
    {
        return id_;
    }

    [[nodiscard]] constexpr auto timestamp() const noexcept -> time::TimePoint
    {
        return timestamp_;
    }

    [[nodiscard]] constexpr auto type() const noexcept -> EventType
    {
        return type_;
    }

    [[nodiscard]] constexpr auto causal_predecessor() const noexcept -> EventId
    {
        return causal_predecessor_;
    }

    /// Returns whether this event has a causal predecessor, i.e.
    /// whether causal_predecessor() is not the nil EventId.
    [[nodiscard]] constexpr auto has_causal_predecessor() const noexcept -> bool
    {
        return !causal_predecessor_.is_nil();
    }

    [[nodiscard]] friend auto operator==(const Event& lhs, const Event& rhs) noexcept -> bool
    {
        return lhs.id_ == rhs.id_ && lhs.timestamp_ == rhs.timestamp_ && lhs.type_ == rhs.type_ &&
               lhs.causal_predecessor_ == rhs.causal_predecessor_;
    }

private:
    EventId id_;
    time::TimePoint timestamp_;
    EventType type_;
    EventId causal_predecessor_;
};

} // namespace nexus::core::events
