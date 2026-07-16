// ==========================================
// nexus::core — Logging: Log Levels
// ==========================================
//
// Compile-time and runtime severity levels for the Logging Framework
// (Phase 8C). Pure data — no I/O, no sinks, no dependency on Logger.

#pragma once

#include "nexus/core/types/types.hpp"

#include <string_view>

namespace nexus::core
{

/// Severity level associated with a single log record.
enum class LogLevel : U8
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off
};

/// Returns a fixed, human-readable name for `level`.
[[nodiscard]] constexpr auto to_string(LogLevel level) noexcept -> std::string_view
{
    switch (level)
    {
        case LogLevel::Trace:
            return "TRACE";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Critical:
            return "CRITICAL";
        case LogLevel::Off:
            return "OFF";
    }
    return "UNKNOWN";
}

} // namespace nexus::core
