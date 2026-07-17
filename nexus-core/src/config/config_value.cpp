// Implementation of ConfigurationValue (Phase 8G).

#include "nexus/core/config/config_value.hpp"

#include <format>
#include <utility>
#include <variant>

namespace nexus::core {

ConfigurationValue::ConfigurationValue(bool value) noexcept : storage_{value} {}

ConfigurationValue::ConfigurationValue(std::int64_t value) noexcept : storage_{value} {}

ConfigurationValue::ConfigurationValue(double value) noexcept : storage_{value} {}

ConfigurationValue::ConfigurationValue(std::string value) : storage_{std::move(value)} {}

ConfigurationValue::ConfigurationValue(std::string_view value) : storage_{std::string{value}} {}

auto ConfigurationValue::type() const noexcept -> ConfigValueType
{
    return static_cast<ConfigValueType>(storage_.index());
}

auto ConfigurationValue::is_bool() const noexcept -> bool
{
    return std::holds_alternative<bool>(storage_);
}

auto ConfigurationValue::is_int() const noexcept -> bool
{
    return std::holds_alternative<std::int64_t>(storage_);
}

auto ConfigurationValue::is_float() const noexcept -> bool
{
    return std::holds_alternative<double>(storage_);
}

auto ConfigurationValue::is_string() const noexcept -> bool
{
    return std::holds_alternative<std::string>(storage_);
}

namespace {

[[nodiscard]] auto type_mismatch_error(ConfigValueType actual, ConfigValueType requested) -> Error
{
    return Error{ErrorCode::ConfigInvalid,
                 std::format("ConfigurationValue holds {} but {} was requested", to_string(actual),
                             to_string(requested))};
}

} // namespace

auto ConfigurationValue::as_bool() const -> Result<bool>
{
    if (const auto* value = std::get_if<bool>(&storage_)) {
        return *value;
    }
    return type_mismatch_error(type(), ConfigValueType::Bool);
}

auto ConfigurationValue::as_int() const -> Result<std::int64_t>
{
    if (const auto* value = std::get_if<std::int64_t>(&storage_)) {
        return *value;
    }
    return type_mismatch_error(type(), ConfigValueType::Int);
}

auto ConfigurationValue::as_float() const -> Result<double>
{
    if (const auto* value = std::get_if<double>(&storage_)) {
        return *value;
    }
    return type_mismatch_error(type(), ConfigValueType::Float);
}

auto ConfigurationValue::as_string() const -> Result<std::string>
{
    if (const auto* value = std::get_if<std::string>(&storage_)) {
        return *value;
    }
    return type_mismatch_error(type(), ConfigValueType::String);
}

auto operator==(const ConfigurationValue& lhs, const ConfigurationValue& rhs) noexcept -> bool
{
    return lhs.storage_ == rhs.storage_;
}

auto to_string(const ConfigurationValue& value) -> std::string
{
    return value.visit([]<typename T>(const T& held) -> std::string {
        if constexpr (std::is_same_v<T, bool>) {
            return held ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::format("\"{}\"", held);
        } else {
            return std::format("{}", held);
        }
    });
}

} // namespace nexus::core
