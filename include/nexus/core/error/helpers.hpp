// ==========================================
// nexus::core — Error Handling: Helper Utilities
// ==========================================
//
// Small free functions that make Result<T>/Error construction sites
// read naturally (success(x), failure(code, msg)) instead of relying
// on Result's converting constructors directly. Result<T> already
// exposes has_value()/value_or() as members (Step 4); the free
// functions here exist only where a non-member form is genuinely
// more ergonomic, and forward straight to those members rather than
// duplicating logic.

#pragma once

#include "nexus/core/error/error.hpp"
#include "nexus/core/error/result.hpp"

#include <source_location>
#include <string>
#include <utility>

namespace nexus::core {

/// Constructs a successful Result<T> from `value`.
template<typename T> [[nodiscard]] constexpr auto success(T value) -> Result<T>
{
    return Result<T>{std::move(value)};
}

/// Constructs a failed Result<T> from an ErrorCode and message.
template<typename T>
[[nodiscard]] auto failure(ErrorCode code, std::string message,
                           std::source_location location = std::source_location::current())
    -> Result<T>
{
    return Result<T>{Error{code, std::move(message), location}};
}

/// Constructs a failed Result<T> from an existing Error.
template<typename T> [[nodiscard]] constexpr auto failure(Error error) -> Result<T>
{
    return Result<T>{std::move(error)};
}

/// Returns whether `result` holds a value. Equivalent to
/// `result.has_value()`; provided as a free function for use in
/// generic/algorithmic contexts (e.g. passed as a predicate).
template<typename T>
[[nodiscard]] constexpr auto has_value(const Result<T>& result) noexcept -> bool
{
    return result.has_value();
}

/// Returns `result`'s value, or `fallback` if it holds an Error.
/// Equivalent to `result.value_or(fallback)`; provided as a free
/// function for symmetry with success()/failure()/error().
template<typename T> [[nodiscard]] constexpr auto value_or(const Result<T>& result, T fallback) -> T
{
    return result.value_or(std::move(fallback));
}

/// Returns `result`'s Error. Precondition: `!result.has_value()`.
template<typename T>
[[nodiscard]] constexpr auto error(const Result<T>& result) noexcept -> const Error&
{
    return result.error();
}

} // namespace nexus::core
