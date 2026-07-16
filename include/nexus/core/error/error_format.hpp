// ==========================================
// nexus::core — Error Handling: Basic Formatting
// ==========================================
//
// Canonical single-line rendering of an Error, mirroring the
// Logging Framework's format_log_record (Phase 8C) so error text
// reads consistently wherever it is logged or surfaced.

#pragma once

#include "nexus/core/error/error.hpp"

#include <string>

namespace nexus::core {

/// Produces the canonical single-line representation of `error`:
/// "[Category/Code] file:line: message".
[[nodiscard]] auto format_error(const Error& error) -> std::string;

} // namespace nexus::core
