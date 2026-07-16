// ==========================================
// nexus::core — Logging: Compile-Time Filtering
// ==========================================
//
// NEXUS_LOG_ACTIVE_LEVEL controls which severities are compiled in
// at all. It is a build-time floor beneath Logger's runtime filter:
// records below this threshold generate no code, so they cost
// nothing even if runtime filtering were misconfigured. Define
// NEXUS_LOG_ACTIVE_LEVEL before including this header (e.g. via a
// compile definition) to override the Debug-build default below.

#pragma once

#include "nexus/core/logging/log_level.hpp"
#include "nexus/core/logging/logger.hpp"

namespace nexus::core::detail {

#if !defined(NEXUS_LOG_ACTIVE_LEVEL)
    #if defined(NDEBUG)
        #define NEXUS_LOG_ACTIVE_LEVEL 2 // Info
    #else
        #define NEXUS_LOG_ACTIVE_LEVEL 0 // Trace
    #endif
#endif

/// Compile-time gate: true when `level` is at or above the
/// active compile-time floor set by NEXUS_LOG_ACTIVE_LEVEL.
[[nodiscard]] constexpr auto is_compiled_in(LogLevel level) noexcept -> bool
{
    return static_cast<int>(level) >= NEXUS_LOG_ACTIVE_LEVEL;
}

} // namespace nexus::core::detail

#define NEXUS_LOG_IMPL(logger, level, message)                                                     \
    do {                                                                                           \
        if constexpr (::nexus::core::detail::is_compiled_in(level)) {                              \
            (logger).log((level), (message));                                                      \
        }                                                                                          \
    } while (false)

#define NEXUS_LOG_TRACE(logger, message)                                                           \
    NEXUS_LOG_IMPL(logger, ::nexus::core::LogLevel::Trace, message)
#define NEXUS_LOG_DEBUG(logger, message)                                                           \
    NEXUS_LOG_IMPL(logger, ::nexus::core::LogLevel::Debug, message)
#define NEXUS_LOG_INFO(logger, message)                                                            \
    NEXUS_LOG_IMPL(logger, ::nexus::core::LogLevel::Info, message)
#define NEXUS_LOG_WARN(logger, message)                                                            \
    NEXUS_LOG_IMPL(logger, ::nexus::core::LogLevel::Warn, message)
#define NEXUS_LOG_ERROR(logger, message)                                                           \
    NEXUS_LOG_IMPL(logger, ::nexus::core::LogLevel::Error, message)
#define NEXUS_LOG_CRITICAL(logger, message)                                                        \
    NEXUS_LOG_IMPL(logger, ::nexus::core::LogLevel::Critical, message)
