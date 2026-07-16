// ==========================================
// nexus::core — Identity: UUID
// ==========================================
//
// A 128-bit UUID value type (RFC 4122 layout). This header covers
// construction, the nil UUID, equality, and ordering only; string
// conversion lives in uuid_format.hpp and hashing in uuid_hash.hpp
// to keep each concern independently includable, matching the
// Logging Framework's split between logger.hpp and log_format.hpp.
//
// No generation facility is provided here — ChaosNexus's determinism
// guarantees (00_PROJECT_CHARTER.md, Section 16) mean UUID
// generation must be seeded/deterministic per simulation run, which
// is a concern of a future phase's ID-generation service, not of the
// UUID value type itself.

#pragma once

#include "nexus/core/types/types.hpp"

#include <algorithm>
#include <array>
#include <compare>

namespace nexus::core {

/// A 128-bit UUID value (RFC 4122 layout), stored as 16 bytes in
/// network byte order. Trivially copyable, regular, and totally
/// ordered by byte value.
class Uuid {
public:
    using ByteArray = std::array<types::U8, 16>;

    constexpr Uuid() noexcept = default;

    explicit constexpr Uuid(const ByteArray& bytes) noexcept : bytes_{bytes} {}

    /// Returns the canonical all-zero UUID (RFC 4122 Nil UUID).
    [[nodiscard]] static constexpr auto nil() noexcept -> Uuid
    {
        return Uuid{};
    }

    /// Returns whether this UUID equals the Nil UUID.
    [[nodiscard]] constexpr auto is_nil() const noexcept -> bool
    {
        return std::ranges::all_of(bytes_, [](types::U8 byte) { return byte == 0; });
    }

    [[nodiscard]] constexpr auto bytes() const noexcept -> const ByteArray&
    {
        return bytes_;
    }

    [[nodiscard]] friend constexpr auto operator==(const Uuid&, const Uuid&) noexcept
        -> bool = default;

    [[nodiscard]] friend constexpr auto operator<=>(const Uuid& lhs, const Uuid& rhs) noexcept
        -> std::strong_ordering
    {
        for (types::Size i = 0; i < lhs.bytes_.size(); ++i) {
            if (lhs.bytes_[i] != rhs.bytes_[i]) {
                return lhs.bytes_[i] <=> rhs.bytes_[i];
            }
        }
        return std::strong_ordering::equal;
    }

private:
    ByteArray bytes_{};
};

} // namespace nexus::core
