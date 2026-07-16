// ==========================================
// nexus::core — Identity: Helper Utilities
// ==========================================
//
// Free-function forms of StrongId<Tag>'s members, mirroring the
// Error Handling Framework's helpers.hpp pattern (Phase 8D:
// has_value()/value_or()/error() alongside Result<T>'s own
// members). Provided for use in generic/algorithmic contexts and
// for symmetry with uuid_from_string(); each simply forwards to the
// corresponding StrongId/Uuid member or free function.

#pragma once

#include "nexus/core/error/result.hpp"
#include "nexus/core/identity/strong_id.hpp"
#include "nexus/core/identity/uuid.hpp"
#include "nexus/core/identity/uuid_format.hpp"

#include <string_view>

namespace nexus::core {

/// Returns whether `id` is the nil identifier. Equivalent to
/// `id.is_nil()`.
template<typename Tag> [[nodiscard]] constexpr auto is_nil(const StrongId<Tag>& id) noexcept -> bool
{
    return id.is_nil();
}

/// Returns the underlying Uuid of `id`. Equivalent to `id.value()`.
template<typename Tag> [[nodiscard]] constexpr auto value(const StrongId<Tag>& id) noexcept -> Uuid
{
    return id.value();
}

/// Parses `text` as a canonical UUID string and wraps the result as
/// a StrongId<Tag>. Failure is forwarded unchanged from
/// uuid_from_string().
template<typename Tag>
[[nodiscard]] auto from_string(std::string_view text) -> Result<StrongId<Tag>>
{
    Result<Uuid> parsed = uuid_from_string(text);
    if (!parsed.has_value()) {
        return Result<StrongId<Tag>>{std::move(parsed).error()};
    }
    return Result<StrongId<Tag>>{StrongId<Tag>{parsed.value()}};
}

} // namespace nexus::core
