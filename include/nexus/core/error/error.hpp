// ==========================================
// nexus::core — Error Handling: Error Object
// ==========================================
//
// Lightweight value type describing a single failure: what went
// wrong (ErrorCode/ErrorCategory), a human-readable message, and
// where it was raised. No exceptions, no dynamic dispatch — this is
// the payload Result<T> carries on its failure path.

#pragma once

#include "nexus/core/error/error_category.hpp"
#include "nexus/core/error/error_code.hpp"

#include <source_location>
#include <string>
#include <string_view>
#include <utility>

namespace nexus::core {

/// Describes a single error condition: code, derived category, a
/// message, and the source location where it was raised.
class Error {
public:
    Error(ErrorCode code, std::string message,
          std::source_location location = std::source_location::current())
        : code_{code}, message_{std::move(message)}, location_{location}
    {}

    [[nodiscard]] auto code() const noexcept -> ErrorCode
    {
        return code_;
    }

    [[nodiscard]] auto category() const noexcept -> ErrorCategory
    {
        return category_of(code_);
    }

    [[nodiscard]] auto message() const noexcept -> std::string_view
    {
        return message_;
    }

    [[nodiscard]] auto location() const noexcept -> const std::source_location&
    {
        return location_;
    }

private:
    ErrorCode code_;
    std::string message_;
    std::source_location location_;
};

} // namespace nexus::core
