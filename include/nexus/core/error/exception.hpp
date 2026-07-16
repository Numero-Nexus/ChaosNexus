// ==========================================
// nexus::core — Error Handling: Exception Hierarchy
// ==========================================
//
// Exception types for the (comparatively rare) cases where
// ChaosNexus code needs to unwind via C++ exceptions rather than
// return a Result<T> — e.g. constructors, or boundaries where a
// Result-based signature isn't viable. Every exception carries an
// Error, so callers get the same ErrorCode/ErrorCategory/message/
// location payload regardless of which propagation style was used.
// NexusException is the sole root; derived types narrow by
// ErrorCategory, mirroring the category groupings in
// error_category.hpp.

#pragma once

#include "nexus/core/error/error.hpp"
#include "nexus/core/error/error_format.hpp"

#include <exception>
#include <utility>

namespace nexus::core {

/// Root of the ChaosNexus exception hierarchy. Wraps an Error so the
/// same structured information (code, category, message, location)
/// is available whether a failure was reported via Result<T> or
/// thrown as an exception.
class NexusException : public std::exception {
public:
    explicit NexusException(Error error)
        : error_{std::move(error)}, formatted_{format_error(error_)}
    {}

    [[nodiscard]] auto error() const noexcept -> const Error&
    {
        return error_;
    }

    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return formatted_.c_str();
    }

private:
    Error error_;
    std::string formatted_;
};

/// Thrown for ErrorCategory::Runtime failures (invalid state,
/// operation failures, timeouts).
class RuntimeException : public NexusException {
public:
    using NexusException::NexusException;
};

/// Thrown for ErrorCategory::Configuration failures (missing,
/// invalid, or unparseable configuration).
class ConfigurationException : public NexusException {
public:
    using NexusException::NexusException;
};

/// Thrown for ErrorCategory::Internal failures — invariant
/// violations and conditions that should be unreachable in correct
/// code.
class InternalException : public NexusException {
public:
    using NexusException::NexusException;
};

} // namespace nexus::core
