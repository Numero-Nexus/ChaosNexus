// Implementation of the Error formatting helper (Phase 8D).

#include "nexus/core/error/error_format.hpp"

#include <format>

namespace nexus::core {

auto format_error(const Error& error) -> std::string
{
    return std::format("[{}/{}] {}:{}: {}", to_string(error.category()), to_string(error.code()),
                       error.location().file_name(), error.location().line(), error.message());
}

} // namespace nexus::core
