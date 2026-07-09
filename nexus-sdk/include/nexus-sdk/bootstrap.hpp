// nexus-sdk public bootstrap header.
//
// nexus-sdk is the extension interface layer through which nexus-core
// is extended, without exposing internal engine implementation detail.
// This header exists only to verify the module's include path is
// correctly wired during repository bootstrap (Phase 7A). No SDK
// surface is defined yet.

#pragma once

namespace nexus::sdk {

// Build-verification anchor only. Not part of the eventual SDK surface.
inline constexpr bool bootstrap_ok = true;

} // namespace nexus::sdk