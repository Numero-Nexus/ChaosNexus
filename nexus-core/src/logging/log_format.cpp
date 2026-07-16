// Implementation of the Basic Formatting helper (Phase 8C).

#include "nexus/core/logging/log_format.hpp"

#include <format>

namespace nexus::core {

auto format_log_record(LogLevel level, std::string_view message,
                       const std::source_location& location) -> std::string
{
    return std::format("[{}] {}:{}: {}", to_string(level), location.file_name(), location.line(),
                       message);
}

} // namespace nexus::core
