// ==========================================
// nexus::core — Logging: Logger
// ==========================================
//
// Central fan-out point for log records: applies runtime level
// filtering, formats the record, and forwards it to every registered
// sink. Thread-safe — sink registration and log() may be called
// concurrently from multiple threads.

#pragma once

#include "nexus/core/logging/log_level.hpp"
#include "nexus/core/logging/log_sink.hpp"

#include <memory>
#include <mutex>
#include <source_location>
#include <string_view>
#include <vector>

namespace nexus::core
{

/// Owns a set of LogSinks and dispatches formatted records to each
/// of them, subject to a runtime-configurable minimum LogLevel.
class Logger
{
public:
    explicit Logger(LogLevel min_level = LogLevel::Info);

    /// Registers `sink` for future log records. Thread-safe.
    auto add_sink(std::shared_ptr<LogSink> sink) -> void;

    /// Sets the runtime-filtered minimum level. Records below this
    /// level are dropped before formatting. Thread-safe.
    auto set_level(LogLevel level) noexcept -> void;

    /// Returns the current runtime minimum level.
    [[nodiscard]] auto level() const noexcept -> LogLevel;

    /// Returns whether `level` currently passes runtime filtering.
    [[nodiscard]] auto is_enabled(LogLevel level) const noexcept -> bool;

    /// Formats and dispatches a single log record to all sinks, if
    /// `level` passes runtime filtering.
    auto log(LogLevel level, std::string_view message,
              std::source_location location = std::source_location::current()) -> void;

private:
    mutable std::mutex mutex_;
    LogLevel min_level_;
    std::vector<std::shared_ptr<LogSink>> sinks_;
};

} // namespace nexus::core
