// ==========================================
// nexus::core — Error Handling: Assertions
// ==========================================
//
// Two assertion tiers:
//   - NEXUS_ASSERT / NEXUS_ASSERT_MSG: debug-only invariant checks,
//     compiled out entirely under NDEBUG (zero release overhead).
//   - NEXUS_VERIFY / NEXUS_VERIFY_MSG: always active; throws
//     InternalException on failure rather than aborting, so release
//     builds fail safely instead of silently continuing.
// Both tiers log a Critical record before failing if a Logger has
// been registered via set_assertion_logger(), integrating with the
// Phase 8C Logging Framework without forcing every translation unit
// to thread a Logger& through call sites.

#pragma once

#include "nexus/core/error/error.hpp"
#include "nexus/core/error/exception.hpp"
#include "nexus/core/logging/logger.hpp"

#include <cstdlib>
#include <source_location>
#include <string_view>

namespace nexus::core::detail {

/// Process-wide logger used by assertion failures, if any. Not
/// owned: registered via set_assertion_logger(), which the owner of
/// the Logger instance is responsible for outliving assertion use.
inline Logger* g_assertion_logger =
    nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/// Logs `message` at Critical severity via the registered assertion
/// logger, if one has been set. No-op otherwise.
inline auto log_assertion_failure(std::string_view message, const std::source_location& location)
    -> void
{
    if (g_assertion_logger != nullptr) {
        g_assertion_logger->log(LogLevel::Critical, message, location);
    }
}

} // namespace nexus::core::detail

namespace nexus::core {

/// Registers the Logger assertion failures report to. Pass nullptr
/// to disable logging on assertion failure.
inline auto set_assertion_logger(Logger* logger) noexcept -> void
{
    detail::g_assertion_logger = logger;
}

} // namespace nexus::core

#define NEXUS_ASSERT_IMPL(condition, message, location)                                            \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            ::nexus::core::detail::log_assertion_failure((message), (location));                   \
            std::abort();                                                                          \
        }                                                                                          \
    } while (false)

#define NEXUS_VERIFY_IMPL(condition, message, location)                                            \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            ::nexus::core::detail::log_assertion_failure((message), (location));                   \
            throw ::nexus::core::InternalException{                                                \
                ::nexus::core::Error{::nexus::core::ErrorCode::PreconditionViolated,               \
                                     std::string{(message)}, (location)}};                         \
        }                                                                                          \
    } while (false)

#if defined(NDEBUG)
    #define NEXUS_ASSERT(condition) ((void)0)
    #define NEXUS_ASSERT_MSG(condition, message) ((void)0)
#else
    #define NEXUS_ASSERT(condition)                                                                \
        NEXUS_ASSERT_IMPL(condition, #condition, std::source_location::current())
    #define NEXUS_ASSERT_MSG(condition, message)                                                   \
        NEXUS_ASSERT_IMPL(condition, message, std::source_location::current())
#endif

#define NEXUS_VERIFY(condition)                                                                    \
    NEXUS_VERIFY_IMPL(condition, #condition, std::source_location::current())
#define NEXUS_VERIFY_MSG(condition, message)                                                       \
    NEXUS_VERIFY_IMPL(condition, message, std::source_location::current())
