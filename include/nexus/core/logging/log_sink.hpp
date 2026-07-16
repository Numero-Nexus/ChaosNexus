// ==========================================
// nexus::core — Logging: Sink Interface
// ==========================================
//
// Abstract destination for formatted log records. Sinks own no
// Logger state; the Logger owns sinks via non-owning references
// registered as shared_ptr<LogSink> to permit safe fan-out without
// raw owning pointers (see Coding Standards: no raw owning pointers).

#pragma once

#include "nexus/core/logging/log_level.hpp"

#include <string_view>

namespace nexus::core {

/// Interface implemented by all log destinations (console, file,
/// network, ...). Implementations must be thread-safe: `write` may
/// be called concurrently from multiple threads sharing a Logger.
class LogSink {
public:
    LogSink() = default;
    LogSink(const LogSink&) = delete;
    LogSink(LogSink&&) = delete;
    auto operator=(const LogSink&) -> LogSink& = delete;
    auto operator=(LogSink&&) -> LogSink& = delete;
    virtual ~LogSink() = default;

    /// Writes a single already-formatted log record to this sink.
    /// `level` is provided alongside the formatted text so sinks may
    /// apply level-specific presentation (e.g. colorization).
    virtual auto write(LogLevel level, std::string_view formatted) -> void = 0;
};

} // namespace nexus::core
