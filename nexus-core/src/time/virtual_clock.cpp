#include "nexus/core/time/virtual_clock.hpp"

#include "nexus/core/error/assert.hpp"

namespace nexus::core::time {

auto VirtualClock::advance(TimePoint target) -> void
{
    NEXUS_VERIFY_MSG(
        target >= current_,
        "VirtualClock::advance: target precedes current time (monotonicity violation)");
    current_ = target;
}

} // namespace nexus::core::time
