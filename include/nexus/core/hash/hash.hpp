// ==========================================
// nexus::core::hash — Generic Hash Utilities
// ==========================================
//
// Composable, allocation-free hashing primitives shared across
// ChaosNexus: a Boost-style hash_combine(), range hashing, and
// std::hash support for std::pair/std::tuple/std::array/enums.
// Existing per-type specializations (Uuid, StrongId<Tag>) are
// unaffected and continue to live in identity/uuid_hash.hpp.
//
// Layering: this header is part of the Core Utilities layer. It must
// never depend on the Simulation Engine, SDK, CLI, or Plugins.

#pragma once

#include "nexus/core/types/concepts.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nexus::core::hash {

/// Satisfied by any type usable as a std::hash<T> key.
template<typename T>
concept Hashable = requires(const std::remove_cvref_t<T>& value) {
    { std::hash<std::remove_cvref_t<T>>{}(value) } -> std::convertible_to<std::size_t>;
};

/// Combines `seed` with the hash of each value in `values`, in order,
/// via a Boost-style mix. Order-sensitive: hash_combine(s, a, b) !=
/// hash_combine(s, b, a) in general.
[[nodiscard]] auto hash_combine(std::size_t seed, const Hashable auto&... values) noexcept
    -> std::size_t
{
    // Boost-style hash_combine mix, folded left-to-right over the
    // argument pack. Mirrors the two-value mix already inlined in
    // identity/uuid_hash.hpp; that file is frozen this phase and is
    // not being changed to delegate here.
    ((seed ^= std::hash<std::remove_cvref_t<decltype(values)>>{}(values) + 0x9E3779B97F4A7C15ULL +
              (seed << 6) + (seed >> 2)),
     ...);
    return seed;
}

/// Combines the hashes of every element in [first, last), in
/// iteration order, starting from a fixed seed. Empty ranges return
/// the seed unmodified.
template<std::input_iterator It, std::sentinel_for<It> S>
    requires Hashable<std::iter_value_t<It>>
[[nodiscard]] auto hash_range(It first, S last) noexcept -> std::size_t
{
    std::size_t seed = 0;
    for (; first != last; ++first) {
        seed = hash_combine(seed, *first);
    }
    return seed;
}

/// Range-based overload of hash_range(first, last).
template<std::ranges::input_range R>
    requires Hashable<std::ranges::range_value_t<R>>
[[nodiscard]] auto hash_range(const R& range) noexcept -> std::size_t
{
    return hash_range(std::ranges::begin(range), std::ranges::end(range));
}

/// Heterogeneous ("transparent") hash functor for string-like keys,
/// enabling unordered_map<std::string, V, TransparentHash,
/// std::equal_to<>>::find(std::string_view) without constructing a
/// temporary std::string.
struct TransparentHash {
    using is_transparent = void;

    [[nodiscard]] auto operator()(std::string_view value) const noexcept -> std::size_t;
};

/// Hasher functor for std::pair<T1, T2>, combining the hashes of both
/// elements via hash_combine. Not a std::hash specialization — see
/// file header rationale — so opt in explicitly, e.g.
/// std::unordered_map<std::pair<int, int>, V, PairHash>.
template<Hashable T1, Hashable T2> struct PairHash {
    [[nodiscard]] auto operator()(const std::pair<T1, T2>& value) const noexcept -> std::size_t
    {
        return hash_combine(0, value.first, value.second);
    }
};

/// Hasher functor for std::tuple<Ts...>, combining the hashes of
/// every element in order. Opt in explicitly as the Hash template
/// parameter, e.g. std::unordered_set<std::tuple<int, int>, TupleHash<int, int>>.
template<Hashable... Ts> struct TupleHash {
    [[nodiscard]] auto operator()(const std::tuple<Ts...>& value) const noexcept -> std::size_t
    {
        return std::apply([](const Ts&... elems) { return hash_combine(0, elems...); }, value);
    }
};

/// Hasher functor for std::array<T, N>, delegating to hash_range. Opt
/// in explicitly as the Hash template parameter.
template<Hashable T, std::size_t N> struct ArrayHash {
    [[nodiscard]] auto operator()(const std::array<T, N>& value) const noexcept -> std::size_t
    {
        return hash_range(value);
    }
};

/// Hasher functor for any scoped or unscoped enumeration, hashing the
/// underlying integral value. Opt in explicitly as the Hash template
/// parameter, e.g. std::unordered_map<MyEnum, V, EnumHash<MyEnum>>.
template<nexus::core::types::Enum E> struct EnumHash {
    [[nodiscard]] auto operator()(E value) const noexcept -> std::size_t
    {
        using Underlying = std::underlying_type_t<E>;
        return std::hash<Underlying>{}(static_cast<Underlying>(value));
    }
};

} // namespace nexus::core::hash
