// Implementation of ConsoleSink (Phase 8C).

#include "nexus/core/logging/console_sink.hpp"

#include <iostream>

namespace nexus::core {

auto ConsoleSink::write(LogLevel level, std::string_view formatted) -> void
{
    std::scoped_lock lock{mutex_};

    if (level >= LogLevel::Warn) {
        std::cerr << formatted << '\n';
    } else {
        std::cout << formatted << '\n';
    }
}

} // namespace nexus::core
