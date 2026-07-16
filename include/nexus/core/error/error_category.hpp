// ==========================================
// nexus::core — Error Handling: Error Categories
// ==========================================
//
// Coarse grouping for ErrorCode, used for routing/filtering
// diagnostics without switching on every individual code. This
// header is included by error_code.hpp's category_of() declaration,
// so the enum lives here and category_of's definition lives here
// too (out-of-line from error_code.hpp) to avoid a circular include.

#pragma once

#include "nexus/core/error/error_code.hpp"
#include "nexus/core/types/types.hpp"

#include <string_view>

namespace nexus::core {

/// Coarse classification of an ErrorCode's origin/domain.
enum class ErrorCategory : types::U8 {
    General,
    Configuration,
    Io,
    Runtime,
    Internal,
};

/// Returns a fixed, human-readable name for `category`.
[[nodiscard]] constexpr auto to_string(ErrorCategory category) noexcept -> std::string_view
{
    switch (category) {
        case ErrorCategory::General:
            return "General";
        case ErrorCategory::Configuration:
            return "Configuration";
        case ErrorCategory::Io:
            return "Io";
        case ErrorCategory::Runtime:
            return "Runtime";
        case ErrorCategory::Internal:
            return "Internal";
    }
    return "UNKNOWN";
}

[[nodiscard]] constexpr auto category_of(ErrorCode code) noexcept -> ErrorCategory
{
    switch (code) {
        case ErrorCode::None:
        case ErrorCode::Unknown:
        case ErrorCode::InvalidArgument:
        case ErrorCode::NotFound:
        case ErrorCode::AlreadyExists:
        case ErrorCode::Unsupported:
            return ErrorCategory::General;

        case ErrorCode::ConfigMissing:
        case ErrorCode::ConfigInvalid:
        case ErrorCode::ConfigParseFailed:
            return ErrorCategory::Configuration;

        case ErrorCode::IoFailure:
        case ErrorCode::FileNotFound:
        case ErrorCode::PermissionDenied:
            return ErrorCategory::Io;

        case ErrorCode::InvalidState:
        case ErrorCode::OperationFailed:
        case ErrorCode::Timeout:
            return ErrorCategory::Runtime;

        case ErrorCode::InternalError:
        case ErrorCode::NotImplemented:
        case ErrorCode::PreconditionViolated:
            return ErrorCategory::Internal;
    }
    return ErrorCategory::General;
}

} // namespace nexus::core
