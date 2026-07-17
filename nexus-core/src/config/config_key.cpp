// Implementation of ConfigurationKey (Phase 8G).

#include "nexus/core/config/config_key.hpp"

#include <algorithm>
#include <cctype>

namespace nexus::core {

ConfigurationKey::ConfigurationKey(std::string_view name) : name_{name} {}

auto ConfigurationKey::name() const noexcept -> std::string_view
{
    return name_;
}

auto ConfigurationKey::is_valid_name(std::string_view name) noexcept -> bool
{
    if (name.empty()) {
        return false;
    }

    const auto is_head = [](char c) noexcept {
        return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
    };
    const auto is_tail = [](char c) noexcept {
        return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '.' || c == '-';
    };

    if (!is_head(name.front())) {
        return false;
    }

    return std::ranges::all_of(name.substr(1), is_tail);
}

auto to_string(const ConfigurationKey& key) -> std::string
{
    return std::string{key.name()};
}

} // namespace nexus::core

auto std::hash<nexus::core::ConfigurationKey>::operator()(
    const nexus::core::ConfigurationKey& key) const noexcept -> std::size_t
{
    return std::hash<std::string_view>{}(key.name());
}
