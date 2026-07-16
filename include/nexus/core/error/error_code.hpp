// ==========================================
// nexus::core — Error Handling: Error Codes
// ==========================================
//
// Enumerates the stable set of error conditions ChaosNexus code can
// report. Pure data — no exceptions, no allocation, no dependency on
// Error/Result. Category association lives here so Error and
// ErrorCategory can both consume it without a circular include.

#pragma once

#include "nexus/core/types/types.hpp"

#include <string_view>

namespace nexus::core {

enum class ErrorCategory : types::U8;

/// Stable identifier for a specific error condition. Grouped by the
/// ErrorCategory each range belongs to; see `category_of`.
enum class ErrorCode : types::U8 {
    None = 0,

    // General
    Unknown,
    InvalidArgument,
    NotFound,
    AlreadyExists,
    Unsupported,

    // Configuration
    ConfigMissing,
    ConfigInvalid,
    ConfigParseFailed,

    // IO
    IoFailure,
    FileNotFound,
    PermissionDenied,

    // Runtime
    InvalidState,
    OperationFailed,
    Timeout,

    // Internal
    InternalError,
    NotImplemented,
    PreconditionViolated,
};

/// Returns a fixed, human-readable name for `code`.
[[nodiscard]] constexpr auto to_string(ErrorCode code) noexcept -> std::string_view
{
    switch (code) {
        case ErrorCode::None:
            return "None";
        case ErrorCode::Unknown:
            return "Unknown";
        case ErrorCode::InvalidArgument:
            return "InvalidArgument";
        case ErrorCode::NotFound:
            return "NotFound";
        case ErrorCode::AlreadyExists:
            return "AlreadyExists";
        case ErrorCode::Unsupported:
            return "Unsupported";
        case ErrorCode::ConfigMissing:
            return "ConfigMissing";
        case ErrorCode::ConfigInvalid:
            return "ConfigInvalid";
        case ErrorCode::ConfigParseFailed:
            return "ConfigParseFailed";
        case ErrorCode::IoFailure:
            return "IoFailure";
        case ErrorCode::FileNotFound:
            return "FileNotFound";
        case ErrorCode::PermissionDenied:
            return "PermissionDenied";
        case ErrorCode::InvalidState:
            return "InvalidState";
        case ErrorCode::OperationFailed:
            return "OperationFailed";
        case ErrorCode::Timeout:
            return "Timeout";
        case ErrorCode::InternalError:
            return "InternalError";
        case ErrorCode::NotImplemented:
            return "NotImplemented";
        case ErrorCode::PreconditionViolated:
            return "PreconditionViolated";
    }
    return "UNKNOWN";
}

} // namespace nexus::core

// category_of(ErrorCode) is declared and defined in error_category.hpp,
// which must be included after this header wherever the mapping is
// needed. Kept out-of-line here to avoid a circular include between
// ErrorCode and ErrorCategory.
