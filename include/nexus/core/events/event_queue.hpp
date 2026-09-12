// ==========================================
// nexus::core::events — EventQueue
// ==========================================
//
// Deterministic storage and retrieval of not-yet-dispatched Events
// (Phase 10, Step 10H design contract). Orders strictly by
// (timestamp, insertion_sequence), both ascending -- the same total
// order established in Step 10E/ADR-0005. insertion_sequence is
// assigned internally by the queue at push() time, not stored on
// Event itself and not supplied by the caller, so two pushes of an
// otherwise-identical Event are still strictly, deterministically
// ordered by arrival order.
//
// EventQueue is NOT a Scheduler (Master Prompt Section 5): it has no
// dependency on VirtualClock::advance(), no execution loop, and no
// dispatch logic. It only answers "what is the next event, in
// deterministic order" and "remove it."
//
// Backed by a binary heap over std::vector<Entry>, giving O(log n)
// push/pop and O(1) peek -- see Step 10H Section 3 (Complexity
// Requirements). This is an implementation choice, not part of the
// public contract: callers observe only push/pop/peek/size/empty.

#pragma once

#include "nexus/core/error/assert.hpp"
#include "nexus/core/events/event.hpp"
#include "nexus/core/types/types.hpp"

#include <algorithm>
#include <limits>
#include <vector>

namespace nexus::core::events {

/// Deterministic, total-order event queue. Owns every Event it
/// holds; push() transfers ownership in, pop() transfers ownership
/// out. Not copyable-by-design constraint is not imposed here --
/// EventQueue itself may be copied/moved like any other value-owning
/// container, since nothing about it requires reference identity.
class EventQueue {
public:
    EventQueue() = default;

    /// Inserts `event`, assigning it the next insertion-sequence
    /// number. O(log n).
    auto push(Event event) -> void
    {
        NEXUS_VERIFY_MSG(next_sequence_ != std::numeric_limits<types::U64>::max(),
                         "EventQueue: insertion sequence counter overflow");

        entries_.push_back(Entry{.event = event, .sequence = next_sequence_});
        ++next_sequence_;
        std::ranges::push_heap(entries_, is_lower_priority);
    }

    /// Returns whether the queue holds zero events.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return entries_.empty();
    }

    /// Returns the number of events currently held.
    [[nodiscard]] auto size() const noexcept -> types::Size
    {
        return entries_.size();
    }

    /// Returns a read-only reference to the next event in
    /// deterministic order, without removing it. Precondition:
    /// !empty(). The returned reference is invalidated by any
    /// subsequent push()/pop() call.
    [[nodiscard]] auto peek() const -> const Event&
    {
        NEXUS_VERIFY_MSG(!empty(), "EventQueue::peek: queue is empty");
        return entries_.front().event;
    }

    /// Removes and returns the next event in deterministic order.
    /// Precondition: !empty(). O(log n).
    [[nodiscard]] auto pop() -> Event
    {
        NEXUS_VERIFY_MSG(!empty(), "EventQueue::pop: queue is empty");

        std::ranges::pop_heap(entries_, is_lower_priority);
        Event result = entries_.back().event;
        entries_.pop_back();
        return result;
    }

private:
    /// A queued Event paired with the insertion-sequence number
    /// assigned to it at push() time. The sequence number is queue-
    /// owned state (Step 10E/10F decision): it describes "position
    /// of submission into this queue," not an intrinsic property of
    /// the Event value type.
    struct Entry {
        Event event;
        types::U64 sequence;
    };

    /// Heap comparator implementing the (timestamp, sequence) total
    /// order. Returns true when `lhs` has strictly lower priority
    /// than `rhs` (i.e. `lhs` should be popped after `rhs`). Used as
    /// the Compare argument to std::push_heap/std::pop_heap, whose
    /// semantics place the *largest*-under-Compare element at the
    /// front -- inverting the natural order here is what makes the
    /// *smallest* (timestamp, sequence) key surface first.
    [[nodiscard]] static auto is_lower_priority(const Entry& lhs, const Entry& rhs) noexcept -> bool
    {
        if (lhs.event.timestamp() != rhs.event.timestamp()) {
            return lhs.event.timestamp() > rhs.event.timestamp();
        }
        return lhs.sequence > rhs.sequence;
    }

    std::vector<Entry> entries_;
    types::U64 next_sequence_{0};
};

} // namespace nexus::core::events
