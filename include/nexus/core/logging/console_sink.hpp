// ==========================================
// nexus::core — Logging: Console Sink
// ==========================================
//
// LogSink implementation that writes formatted records to stdout
// (or stderr for Error/Critical). Thread-safe via an internal mutex
// serializing writes to the underlying stream.

#pragma once

#include "nexus/core/logging/log_level.hpp"
#include "nexus/core/logging/log_sink.hpp"

#include <mutex>
#include <string_view>

namespace nexus::core {

/// Writes log records to the process standard streams. Info and
/// below go to stdout; Warn and above go to stderr.
class ConsoleSink final : public LogSink {
public:
    ConsoleSink() = default;

    auto write(LogLevel level, std::string_view formatted) -> void override;

private:
    std::mutex mutex_;
};

} // namespace nexus::core
