// ==========================================
// nexus::core — Platform Utilities
// ==========================================
//
// Compile-time platform abstraction for ChaosNexus.
//
// This header provides a single authoritative source of compile-time
// information about the environment ChaosNexus was built for: operating
// system, compiler, CPU architecture, build configuration, endianness,
// pointer size, and the active C++ language standard.
//
// This is strictly a compile-time facility. No runtime platform
// discovery is performed, and nothing in this header may be used to
// influence simulation semantics — doing so would violate the
// determinism guarantees established in 00_PROJECT_CHARTER.md,
// Section 16. This header exists purely to support diagnostics and
// conditional compilation, and has no knowledge of, and no dependency
// on, any simulation concept.
//
// Layering: this header is part of the Core Utilities layer and sits
// at the dependency floor of the entire project. It must never depend
// on the Simulation Engine, SDK, CLI, or Plugins. Higher layers may
// freely depend on it.

#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace nexus::core
{

// ------------------------------------------
// Enumerations
// ------------------------------------------

/// Identifies the operating system ChaosNexus was compiled for.
enum class Platform
{
    Windows,
    Linux,
    MacOS,
    Unknown
};

/// Identifies the compiler ChaosNexus was compiled with.
enum class Compiler
{
    MSVC,
    GCC,
    Clang,
    Unknown
};

/// Identifies the CPU architecture ChaosNexus was compiled for.
enum class Architecture
{
    X86_64,
    ARM64,
    Unknown
};

namespace detail
{

// ------------------------------------------
// Detection (compile-time, preprocessor-driven)
// ------------------------------------------
//
// This is the only region of this header that uses preprocessor
// conditionals. Detection happens exactly once, here, resolving to
// ordinary constexpr values; every public function below consumes
// those resolved values through plain C++ rather than further
// preprocessor branching. Clang is checked before GCC because Clang
// on some platforms defines __GNUC__ for compatibility purposes.

#if defined(_MSC_VER)
inline constexpr Compiler kDetectedCompiler = Compiler::MSVC;
#elif defined(__clang__)
inline constexpr Compiler kDetectedCompiler = Compiler::Clang;
#elif defined(__GNUC__)
inline constexpr Compiler kDetectedCompiler = Compiler::GCC;
#else
inline constexpr Compiler kDetectedCompiler = Compiler::Unknown;
#endif

#if defined(_WIN32)
inline constexpr Platform kDetectedPlatform = Platform::Windows;
#elif defined(__linux__)
inline constexpr Platform kDetectedPlatform = Platform::Linux;
#elif defined(__APPLE__)
inline constexpr Platform kDetectedPlatform = Platform::MacOS;
#else
inline constexpr Platform kDetectedPlatform = Platform::Unknown;
#endif

#if defined(_M_X64) || defined(__x86_64__)
inline constexpr Architecture kDetectedArchitecture = Architecture::X86_64;
#elif defined(_M_ARM64) || defined(__aarch64__)
inline constexpr Architecture kDetectedArchitecture = Architecture::ARM64;
#else
inline constexpr Architecture kDetectedArchitecture = Architecture::Unknown;
#endif

#if defined(NDEBUG)
inline constexpr bool kIsDebugBuild = false;
#else
inline constexpr bool kIsDebugBuild = true;
#endif

} // namespace detail

// ------------------------------------------
// Public API — identity queries
// ------------------------------------------

/// Returns the operating system ChaosNexus was compiled for.
[[nodiscard]] constexpr auto platform() noexcept -> Platform
{
    return detail::kDetectedPlatform;
}

/// Returns the compiler ChaosNexus was compiled with.
[[nodiscard]] constexpr auto compiler() noexcept -> Compiler
{
    return detail::kDetectedCompiler;
}

/// Returns the CPU architecture ChaosNexus was compiled for.
[[nodiscard]] constexpr auto architecture() noexcept -> Architecture
{
    return detail::kDetectedArchitecture;
}

// ------------------------------------------
// Public API — build configuration
// ------------------------------------------

/// Returns true if this is a debug build (NDEBUG is not defined).
[[nodiscard]] constexpr auto is_debug() noexcept -> bool
{
    return detail::kIsDebugBuild;
}

/// Returns true if this is a release build (NDEBUG is defined).
[[nodiscard]] constexpr auto is_release() noexcept -> bool
{
    return !detail::kIsDebugBuild;
}

// ------------------------------------------
// Public API — human-readable names
// ------------------------------------------

/// Returns a human-readable name for the given platform, suitable for
/// diagnostics. Never returns an empty string; unrecognized values
/// (there should be none, since Platform is a closed enum) fall back
/// to "Unknown".
[[nodiscard]] constexpr auto platform_name(Platform value) noexcept -> std::string_view
{
    switch (value)
    {
        case Platform::Windows:
            return "Windows";
        case Platform::Linux:
            return "Linux";
        case Platform::MacOS:
            return "macOS";
        case Platform::Unknown:
            return "Unknown";
    }
    return "Unknown";
}

/// Returns a human-readable name for the current compile-time platform.
[[nodiscard]] constexpr auto platform_name() noexcept -> std::string_view
{
    return platform_name(platform());
}

/// Returns a human-readable name for the given compiler, suitable for
/// diagnostics.
[[nodiscard]] constexpr auto compiler_name(Compiler value) noexcept -> std::string_view
{
    switch (value)
    {
        case Compiler::MSVC:
            return "MSVC";
        case Compiler::GCC:
            return "GCC";
        case Compiler::Clang:
            return "Clang";
        case Compiler::Unknown:
            return "Unknown";
    }
    return "Unknown";
}

/// Returns a human-readable name for the current compile-time compiler.
[[nodiscard]] constexpr auto compiler_name() noexcept -> std::string_view
{
    return compiler_name(compiler());
}

/// Returns a human-readable name for the given architecture, suitable
/// for diagnostics.
[[nodiscard]] constexpr auto architecture_name(Architecture value) noexcept -> std::string_view
{
    switch (value)
    {
        case Architecture::X86_64:
            return "x86_64";
        case Architecture::ARM64:
            return "ARM64";
        case Architecture::Unknown:
            return "Unknown";
    }
    return "Unknown";
}

/// Returns a human-readable name for the current compile-time
/// architecture.
[[nodiscard]] constexpr auto architecture_name() noexcept -> std::string_view
{
    return architecture_name(architecture());
}

// ------------------------------------------
// Public API — language standard, pointer size, endianness
// ------------------------------------------

/// Returns the active C++ language standard as its __cplusplus-style
/// year value (e.g. 202302 for C++23).
[[nodiscard]] constexpr auto cpp_standard() noexcept -> std::uint32_t
{
    return static_cast<std::uint32_t>(__cplusplus);
}

/// Returns the size, in bytes, of a native pointer on this platform.
[[nodiscard]] constexpr auto pointer_size() noexcept -> std::size_t
{
    return sizeof(void*);
}

/// Returns the native byte order of the target platform.
[[nodiscard]] constexpr auto native_endian() noexcept -> std::endian
{
    return std::endian::native;
}

} // namespace nexus::core
