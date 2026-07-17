// ==========================================
// nexus::core — Configuration: Configuration Value
// ==========================================
//
// A closed-set, strongly typed value for configuration data: bool,
// signed 64-bit integer, double, or string, held in a std::variant.
// No parsing/coercion between types is performed (e.g. the string
// "42" does not become an Int) — type-safe accessors either return
// the stored value or an Error{ErrorCode::ConfigInvalid, ...}. This
// header declares the public surface only; constructors, accessors,
// visitation, comparison, and string conversion are implemented in
// Step 2.

#pragma once

#include "nexus/core/error/result.hpp"
#include "nexus/core/types/types.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace nexus::core {

/// Discriminant for the type currently held by a ConfigurationValue.
enum class ConfigValueType : types::U8 {
    Bool,
    Int,
    Float,
    String,
};

/// Returns a fixed, human-readable name for `type`.
[[nodiscard]] constexpr auto to_string(ConfigValueType type) noexcept -> std::string_view
{
    switch (type) {
        case ConfigValueType::Bool:
            return "Bool";
        case ConfigValueType::Int:
            return "Int";
        case ConfigValueType::Float:
            return "Float";
        case ConfigValueType::String:
            return "String";
    }
    return "UNKNOWN";
}

/// Underlying closed set of supported primitive value types.
using ConfigVariant = std::variant<bool, std::int64_t, double, std::string>;

/// A single, strongly typed configuration value. Regular value type;
/// copies/moves are exactly as cheap as the underlying std::variant.
class ConfigurationValue {
public:
    explicit ConfigurationValue(bool value) noexcept;
    explicit ConfigurationValue(std::int64_t value) noexcept;
    explicit ConfigurationValue(double value) noexcept;
    explicit ConfigurationValue(std::string value);
    explicit ConfigurationValue(std::string_view value);

    [[nodiscard]] auto type() const noexcept -> ConfigValueType;

    [[nodiscard]] auto is_bool() const noexcept -> bool;
    [[nodiscard]] auto is_int() const noexcept -> bool;
    [[nodiscard]] auto is_float() const noexcept -> bool;
    [[nodiscard]] auto is_string() const noexcept -> bool;

    /// Type-safe accessors. Return Error{ErrorCode::ConfigInvalid, ...}
    /// when the stored type does not match the requested type; never
    /// throw and never silently coerce.
    [[nodiscard]] auto as_bool() const -> Result<bool>;
    [[nodiscard]] auto as_int() const -> Result<std::int64_t>;
    [[nodiscard]] auto as_float() const -> Result<double>;
    [[nodiscard]] auto as_string() const -> Result<std::string>;

    /// Dispatches to `visitor` with the currently held alternative,
    /// forwarding std::visit's return type.
    template<typename Visitor> decltype(auto) visit(Visitor&& visitor) const
    {
        return std::visit(std::forward<Visitor>(visitor), storage_);
    }

    friend auto operator==(const ConfigurationValue& lhs, const ConfigurationValue& rhs) noexcept
        -> bool;

private:
    ConfigVariant storage_;
};

/// Produces a diagnostic string representation of `value` (e.g. for
/// logging), formatted per its held type.
[[nodiscard]] auto to_string(const ConfigurationValue& value) -> std::string;

} // namespace nexus::core
