// ==========================================
// nexus::core — Identity: UUID Parsing & Formatting
// ==========================================
//
// Canonical string conversion for Uuid: the standard
// "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" (8-4-4-4-12 hex, lowercase)
// representation. Parsing returns Result<Uuid> (Phase 8D) rather
// than throwing, since malformed input is an expected, recoverable
// condition rather than a programming error.

#pragma once

#include "nexus/core/error/result.hpp"
#include "nexus/core/identity/uuid.hpp"

#include <format>
#include <string>
#include <string_view>

namespace nexus::core {

/// Produces the canonical lowercase 8-4-4-4-12 hex representation
/// of `id`, e.g. "550e8400-e29b-41d4-a716-446655440000".
[[nodiscard]] auto to_string(const Uuid& id) -> std::string;

/// Parses `text` as a canonical UUID string. Accepts exactly the
/// 8-4-4-4-12 hyphenated hex form (case-insensitive hex digits);
/// any other layout, length, or non-hex character is a parse
/// failure. Returns ErrorCode::ConfigParseFailed on failure — the
/// closest existing code to "malformed textual input"; a
/// dedicated parsing category is left to a future phase.
[[nodiscard]] auto uuid_from_string(std::string_view text) -> Result<Uuid>;

} // namespace nexus::core

// std::formatter specialization so Uuid can be interpolated directly
// into std::format calls — including the messages passed to
// Logger::log() (Phase 8C), which are themselves built with
// std::format. Delegates to to_string() rather than duplicating the
// hex-encoding logic.
template<> struct std::formatter<nexus::core::Uuid> : std::formatter<std::string> {
    auto format(const nexus::core::Uuid& id, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(nexus::core::to_string(id), ctx);
    }
};
