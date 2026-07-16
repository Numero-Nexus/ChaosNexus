// ==========================================
// nexus::core — Error Handling: Result<T>
// ==========================================
//
// Lightweight, move-aware sum type holding either a value of type T
// or an Error. Built directly on std::variant to avoid a bespoke
// storage/union implementation while keeping the public surface
// small and purpose-built for this project's error-handling style.
// No exceptions are thrown by Result itself; value_or_throw() is the
// single, explicit bridge into the Exception hierarchy (Step 5).

#pragma once

#include "nexus/core/error/error.hpp"

#include <concepts>
#include <utility>
#include <variant>

namespace nexus::core {

/// Holds either a value of type T (success) or an Error (failure).
/// T must be movable; Result performs no extra allocation beyond
/// what std::variant<T, Error> itself requires.
template<typename T>
    requires std::move_constructible<T>
class [[nodiscard]] Result {
public:
    using ValueType = T;

    Result(T value) : storage_{std::in_place_index<0>, std::move(value)} {}

    Result(Error error) : storage_{std::in_place_index<1>, std::move(error)} {}

    [[nodiscard]] auto has_value() const noexcept -> bool
    {
        return storage_.index() == 0;
    }

    explicit operator bool() const noexcept
    {
        return has_value();
    }

    [[nodiscard]] auto value() & noexcept -> T&
    {
        return std::get<0>(storage_);
    }
    [[nodiscard]] auto value() const& noexcept -> const T&
    {
        return std::get<0>(storage_);
    }
    [[nodiscard]] auto value() && noexcept -> T&&
    {
        return std::get<0>(std::move(storage_));
    }

    [[nodiscard]] auto error() const& noexcept -> const Error&
    {
        return std::get<1>(storage_);
    }
    [[nodiscard]] auto error() && noexcept -> Error&&
    {
        return std::get<1>(std::move(storage_));
    }

    /// Returns the held value, or `fallback` if this Result holds an
    /// Error. `fallback` is only moved from when actually used.
    [[nodiscard]] auto value_or(T fallback) const& -> T
    {
        return has_value() ? value() : std::move(fallback);
    }

    [[nodiscard]] auto value_or(T fallback) && -> T
    {
        return has_value() ? std::move(*this).value() : std::move(fallback);
    }

private:
    std::variant<T, Error> storage_;
};

} // namespace nexus::core
