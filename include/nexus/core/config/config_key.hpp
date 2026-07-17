// ==========================================
// nexus::core — Configuration: Configuration Key
// ==========================================
//
// A strongly typed, immutable name identifying a configuration
// entry. Wraps std::string rather than std::string_view so a
// ConfigurationKey always owns its name and can outlive whatever
// produced it (a parsed file, a builder, etc.) — consistent with the
// "no parsing/no file-loading" boundary of this phase, which only
// concerns itself with the key as a value type, not where it came
// from. Validation is limited to a static predicate (`is_valid_name`)
// so callers can check before constructing; construction itself does
// not enforce validity, keeping this a plain value type rather than a
// validating wrapper.

#pragma once

#include <compare>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>

namespace nexus::core {

/// Strongly typed name identifying a single configuration entry.
/// Regular, hashable, and totally ordered so it can be used directly
/// as a key in associative containers by future configuration
/// registries.
class ConfigurationKey {
public:
    // A single std::string_view constructor (rather than overloads for
    // both std::string and std::string_view) avoids ambiguity for
    // string-literal/const char* arguments, which convert equally
    // well to either type.
    explicit ConfigurationKey(std::string_view name);

    [[nodiscard]] auto name() const noexcept -> std::string_view;

    /// Returns whether `name` is a syntactically valid configuration
    /// key (non-empty; further rules defined in Step 2). Does not
    /// inspect any existing ConfigurationKey — this is a free-standing
    /// predicate over candidate strings.
    [[nodiscard]] static auto is_valid_name(std::string_view name) noexcept -> bool;

    [[nodiscard]] friend auto operator==(const ConfigurationKey&, const ConfigurationKey&) noexcept
        -> bool = default;

    [[nodiscard]] friend auto operator<=>(const ConfigurationKey&, const ConfigurationKey&) noexcept
        -> std::strong_ordering = default;

private:
    std::string name_;
};

/// Returns the key's name, primarily for diagnostics/logging.
[[nodiscard]] auto to_string(const ConfigurationKey& key) -> std::string;

} // namespace nexus::core

/// std::hash specialization so ConfigurationKey can be used directly
/// in unordered associative containers.
template<> struct std::hash<nexus::core::ConfigurationKey> {
    [[nodiscard]] auto operator()(const nexus::core::ConfigurationKey& key) const noexcept
        -> std::size_t;
};
