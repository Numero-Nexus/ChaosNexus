// ==========================================
// nexus::core — Logging: Basic Formatting
// ==========================================
//
// Pure, allocation-owning formatting helper shared by Logger and
// any future sink that needs the canonical record layout. Kept
// separate from Logger so it can be unit-tested independently.

#pragma once

#include "nexus/core/logging/log_level.hpp"

#include <source_location>
#include <string>
#include <string_view>

namespace nexus::core {

/// Produces the canonical single-line representation of a log
/// record: "[LEVEL] file:line: message".
[[nodiscard]] auto format_log_record(LogLevel level, std::string_view message,
                                     const std::source_location& location) -> std::string;

} // namespace nexus::core
