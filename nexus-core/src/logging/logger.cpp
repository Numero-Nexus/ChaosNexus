// Implementation of Logger (Phase 8C): sink registration, runtime
// filtering, and dispatch.

#include "nexus/core/logging/logger.hpp"

#include "nexus/core/logging/log_format.hpp"

#include <utility>

namespace nexus::core {

Logger::Logger(LogLevel min_level) : min_level_{min_level} {}

auto Logger::add_sink(std::shared_ptr<LogSink> sink) -> void
{
    std::scoped_lock lock{mutex_};
    sinks_.push_back(std::move(sink));
}

auto Logger::set_level(LogLevel level) noexcept -> void
{
    std::scoped_lock lock{mutex_};
    min_level_ = level;
}

auto Logger::level() const noexcept -> LogLevel
{
    std::scoped_lock lock{mutex_};
    return min_level_;
}

auto Logger::is_enabled(LogLevel level) const noexcept -> bool
{
    std::scoped_lock lock{mutex_};
    return level >= min_level_ && min_level_ != LogLevel::Off;
}

auto Logger::log(LogLevel level, std::string_view message, std::source_location location) -> void
{
    std::vector<std::shared_ptr<LogSink>> sinks_copy;

    {
        std::scoped_lock lock{mutex_};
        if (level < min_level_ || min_level_ == LogLevel::Off) {
            return;
        }
        sinks_copy = sinks_;
    }

    const std::string formatted = format_log_record(level, message, location);
    for (const auto& sink : sinks_copy) {
        sink->write(level, formatted);
    }
}

} // namespace nexus::core
