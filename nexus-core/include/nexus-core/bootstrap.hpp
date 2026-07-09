// nexus-core public bootstrap header.
//
// This header exists solely to verify that nexus-core's include path
// is correctly wired into the build system during repository bootstrap
// (Phase 7A). It is not part of the eventual public API surface.

#pragma once

namespace nexus::core {

// Declared in bootstrap.cpp. Build-verification anchor only.
auto bootstrap_ok() noexcept -> bool;

} // namespace nexus::core