// ==========================================
// nexus::core — Identity: Strong Identifier Template
// ==========================================
//
// A zero-overhead, type-safe wrapper around Uuid. Each strong ID
// type is a distinct C++ type (StrongId<NodeTag> != StrongId<EventTag>)
// even though both wrap the same Uuid, preventing accidental mixing
// of identifier domains at compile time. The Tag parameter is never
// instantiated — it exists purely to parameterize the type.
//
// Deliberately not a CRTP base or polymorphic type: StrongId<Tag> is
// a plain aggregate-like wrapper so it stays trivially copyable and
// exactly the size of a Uuid (16 bytes), satisfying the zero-cost
// abstraction requirement.

#pragma once

#include "nexus/core/identity/uuid.hpp"
#include "nexus/core/identity/uuid_format.hpp"
#include "nexus/core/identity/uuid_hash.hpp"

#include <compare>
#include <format>
#include <functional>
#include <string>

namespace nexus::core {

/// Strongly typed identifier wrapping a Uuid. `Tag` is a distinct,
/// never-instantiated type per identifier domain (e.g. a `NodeTag`
/// struct for NodeId) that makes StrongId<Tag> incompatible with
/// StrongId<OtherTag> at compile time despite identical layout.
template<typename Tag> class StrongId {
public:
    using TagType = Tag;

    constexpr StrongId() noexcept = default;

    explicit constexpr StrongId(Uuid value) noexcept : value_{value} {}

    [[nodiscard]] constexpr auto value() const noexcept -> Uuid
    {
        return value_;
    }

    [[nodiscard]] constexpr auto is_nil() const noexcept -> bool
    {
        return value_.is_nil();
    }

    /// Returns a nil-valued StrongId<Tag>, e.g. for "no ID assigned
    /// yet" sentinel states.
    [[nodiscard]] static constexpr auto nil() noexcept -> StrongId<Tag>
    {
        return StrongId<Tag>{Uuid::nil()};
    }

    [[nodiscard]] friend constexpr auto operator==(const StrongId&, const StrongId&) noexcept
        -> bool = default;

    [[nodiscard]] friend constexpr auto operator<=>(const StrongId&, const StrongId&) noexcept
        -> std::strong_ordering = default;

private:
    Uuid value_{};
};

/// Produces the canonical UUID string representation of `id`.
template<typename Tag> [[nodiscard]] auto to_string(const StrongId<Tag>& id) -> std::string
{
    return to_string(id.value());
}

} // namespace nexus::core

/// std::hash specialization for any StrongId<Tag>, delegating to the
/// underlying Uuid's hash so each strong ID type gets hashing "for
/// free" without a per-Tag specialization.
template<typename Tag> struct std::hash<nexus::core::StrongId<Tag>> {
    [[nodiscard]] auto operator()(const nexus::core::StrongId<Tag>& id) const noexcept
        -> std::size_t
    {
        return std::hash<nexus::core::Uuid>{}(id.value());
    }
};

// std::formatter specialization mirroring Uuid's (uuid_format.hpp),
// so any StrongId<Tag> — NodeId, EventId, SimulationId, and future
// domains — can be interpolated directly into std::format calls and
// Logger::log() messages without a per-Tag formatter.
template<typename Tag>
struct std::formatter<nexus::core::StrongId<Tag>> : std::formatter<std::string> {
    auto format(const nexus::core::StrongId<Tag>& id, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(nexus::core::to_string(id), ctx);
    }
};
