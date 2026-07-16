// ==========================================
// nexus::core — Identity: UUID Hashing
// ==========================================
//
// std::hash specialization for Uuid, enabling direct use as a key
// in std::unordered_map/std::unordered_set. Combines the 16 bytes
// as two 64-bit words via a simple mix rather than hashing
// byte-by-byte, since UUID bytes are already high-entropy and a
// per-byte combine would be needlessly slow for the "millions of
// identifiers" performance target.

#pragma once

#include "nexus/core/identity/uuid.hpp"

#include <bit>
#include <cstddef>
#include <cstring>

template<> struct std::hash<nexus::core::Uuid> {
    [[nodiscard]] auto operator()(const nexus::core::Uuid& id) const noexcept -> std::size_t
    {
        const auto& bytes = id.bytes();

        std::uint64_t lo;
        std::uint64_t hi;
        std::memcpy(&lo, bytes.data(), sizeof(lo));
        std::memcpy(&hi, bytes.data() + sizeof(lo), sizeof(hi));

        // Boost-style hash_combine mix of the two 64-bit halves.
        std::uint64_t seed = lo;
        seed ^= hi + 0x9E3779B97F4A7C15ULL + (seed << 6) + (seed >> 2);
        return static_cast<std::size_t>(seed);
    }
};
