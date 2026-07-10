// ==========================================
// nexus::core::platform — Unit Tests
// ==========================================
//
// Verifies the compile-time Platform Utilities subsystem. All checks
// here are deterministic: they either verify a fact about the machine
// actually running the test (guarded by the same preprocessor macros
// the header itself uses) or verify structural properties of the API
// that hold regardless of host platform (e.g. every name function
// returns a non-empty string, is_debug() and is_release() are always
// complementary).

#include "nexus/core/platform/platform.hpp"

#include <gtest/gtest.h>

#include <type_traits>

namespace nexus::core::test
{

// ------------------------------------------
// Operating system detection
// ------------------------------------------

TEST(PlatformDetectionTest, ReportsExpectedPlatformForHostOS)
{
#if defined(_WIN32)
    EXPECT_EQ(platform(), Platform::Windows);
#elif defined(__linux__)
    EXPECT_EQ(platform(), Platform::Linux);
#elif defined(__APPLE__)
    EXPECT_EQ(platform(), Platform::MacOS);
#else
    EXPECT_EQ(platform(), Platform::Unknown);
#endif
}

TEST(PlatformDetectionTest, PlatformIsConstexprEvaluable)
{
    constexpr Platform value = platform();
    EXPECT_EQ(value, platform());
}

// ------------------------------------------
// Compiler detection
// ------------------------------------------

TEST(CompilerDetectionTest, ReportsExpectedCompilerForHostToolchain)
{
#if defined(_MSC_VER)
    EXPECT_EQ(compiler(), Compiler::MSVC);
#elif defined(__clang__)
    EXPECT_EQ(compiler(), Compiler::Clang);
#elif defined(__GNUC__)
    EXPECT_EQ(compiler(), Compiler::GCC);
#else
    EXPECT_EQ(compiler(), Compiler::Unknown);
#endif
}

TEST(CompilerDetectionTest, CompilerIsConstexprEvaluable)
{
    constexpr Compiler value = compiler();
    EXPECT_EQ(value, compiler());
}

// ------------------------------------------
// Architecture detection
// ------------------------------------------

TEST(ArchitectureDetectionTest, ReportsExpectedArchitectureForHostCPU)
{
#if defined(_M_X64) || defined(__x86_64__)
    EXPECT_EQ(architecture(), Architecture::X86_64);
#elif defined(_M_ARM64) || defined(__aarch64__)
    EXPECT_EQ(architecture(), Architecture::ARM64);
#else
    EXPECT_EQ(architecture(), Architecture::Unknown);
#endif
}

TEST(ArchitectureDetectionTest, ArchitectureIsConstexprEvaluable)
{
    constexpr Architecture value = architecture();
    EXPECT_EQ(value, architecture());
}

// ------------------------------------------
// Build configuration
// ------------------------------------------

TEST(BuildConfigurationTest, DebugAndReleaseAreMutuallyExclusive)
{
    EXPECT_NE(is_debug(), is_release());
}

TEST(BuildConfigurationTest, MatchesNDEBUGDefinition)
{
#if defined(NDEBUG)
    EXPECT_TRUE(is_release());
    EXPECT_FALSE(is_debug());
#else
    EXPECT_TRUE(is_debug());
    EXPECT_FALSE(is_release());
#endif
}

TEST(BuildConfigurationTest, IsDebugIsConstexprEvaluable)
{
    constexpr bool value = is_debug();
    EXPECT_EQ(value, is_debug());
}

// ------------------------------------------
// Pointer size
// ------------------------------------------

TEST(PointerSizeTest, MatchesSizeofVoidPointer)
{
    EXPECT_EQ(pointer_size(), sizeof(void*));
}

TEST(PointerSizeTest, IsConstexprEvaluable)
{
    constexpr std::size_t value = pointer_size();
    EXPECT_EQ(value, pointer_size());
}

TEST(PointerSizeTest, IsOneOfExpectedSizesOnSupportedArchitectures)
{
    // Both supported architectures (x86_64, ARM64) are 64-bit.
    if (architecture() == Architecture::X86_64 || architecture() == Architecture::ARM64)
    {
        EXPECT_EQ(pointer_size(), 8U);
    }
}

// ------------------------------------------
// C++ standard
// ------------------------------------------

TEST(CppStandardTest, IsAtLeastCpp23)
{
    // 202302L is the standardized __cplusplus value for C++23.
    EXPECT_GE(cpp_standard(), 202302U);
}

TEST(CppStandardTest, MatchesBuiltinCplusplusMacro)
{
    EXPECT_EQ(cpp_standard(), static_cast<std::uint32_t>(__cplusplus));
}

TEST(CppStandardTest, IsConstexprEvaluable)
{
    constexpr std::uint32_t value = cpp_standard();
    EXPECT_EQ(value, cpp_standard());
}

// ------------------------------------------
// Endianness
// ------------------------------------------

TEST(EndiannessTest, MatchesStdEndianNative)
{
    EXPECT_EQ(native_endian(), std::endian::native);
}

TEST(EndiannessTest, IsEitherLittleOrBigOnSupportedArchitectures)
{
    // x86_64 and ARM64 (in their standard configurations) are little-endian.
    if (architecture() == Architecture::X86_64 || architecture() == Architecture::ARM64)
    {
        EXPECT_EQ(native_endian(), std::endian::little);
    }
}

TEST(EndiannessTest, IsConstexprEvaluable)
{
    constexpr std::endian value = native_endian();
    EXPECT_EQ(value, native_endian());
}

// ------------------------------------------
// Platform name helpers
// ------------------------------------------

TEST(PlatformNameTest, NoArgOverloadMatchesCurrentPlatform)
{
    EXPECT_EQ(platform_name(), platform_name(platform()));
}

TEST(PlatformNameTest, EveryEnumeratorProducesNonEmptyName)
{
    EXPECT_EQ(platform_name(Platform::Windows), "Windows");
    EXPECT_EQ(platform_name(Platform::Linux), "Linux");
    EXPECT_EQ(platform_name(Platform::MacOS), "macOS");
    EXPECT_EQ(platform_name(Platform::Unknown), "Unknown");
}

TEST(PlatformNameTest, IsConstexprEvaluable)
{
    constexpr std::string_view value = platform_name(Platform::Linux);
    EXPECT_EQ(value, "Linux");
}

// ------------------------------------------
// Compiler name helpers
// ------------------------------------------

TEST(CompilerNameTest, NoArgOverloadMatchesCurrentCompiler)
{
    EXPECT_EQ(compiler_name(), compiler_name(compiler()));
}

TEST(CompilerNameTest, EveryEnumeratorProducesNonEmptyName)
{
    EXPECT_EQ(compiler_name(Compiler::MSVC), "MSVC");
    EXPECT_EQ(compiler_name(Compiler::GCC), "GCC");
    EXPECT_EQ(compiler_name(Compiler::Clang), "Clang");
    EXPECT_EQ(compiler_name(Compiler::Unknown), "Unknown");
}

TEST(CompilerNameTest, IsConstexprEvaluable)
{
    constexpr std::string_view value = compiler_name(Compiler::Clang);
    EXPECT_EQ(value, "Clang");
}

// ------------------------------------------
// Architecture name helpers
// ------------------------------------------

TEST(ArchitectureNameTest, NoArgOverloadMatchesCurrentArchitecture)
{
    EXPECT_EQ(architecture_name(), architecture_name(architecture()));
}

TEST(ArchitectureNameTest, EveryEnumeratorProducesNonEmptyName)
{
    EXPECT_EQ(architecture_name(Architecture::X86_64), "x86_64");
    EXPECT_EQ(architecture_name(Architecture::ARM64), "ARM64");
    EXPECT_EQ(architecture_name(Architecture::Unknown), "Unknown");
}

TEST(ArchitectureNameTest, IsConstexprEvaluable)
{
    constexpr std::string_view value = architecture_name(Architecture::ARM64);
    EXPECT_EQ(value, "ARM64");
}

// ------------------------------------------
// Unknown / fallback handling
// ------------------------------------------

TEST(UnknownFallbackTest, UnknownPlatformNameIsExplicit)
{
    EXPECT_EQ(platform_name(Platform::Unknown), "Unknown");
    EXPECT_NE(platform_name(Platform::Unknown), "");
}

TEST(UnknownFallbackTest, UnknownCompilerNameIsExplicit)
{
    EXPECT_EQ(compiler_name(Compiler::Unknown), "Unknown");
    EXPECT_NE(compiler_name(Compiler::Unknown), "");
}

TEST(UnknownFallbackTest, UnknownArchitectureNameIsExplicit)
{
    EXPECT_EQ(architecture_name(Architecture::Unknown), "Unknown");
    EXPECT_NE(architecture_name(Architecture::Unknown), "");
}

// ------------------------------------------
// Static structural guarantees (compile-time only, no runtime assertion)
// ------------------------------------------

static_assert(std::is_enum_v<Platform>);
static_assert(std::is_enum_v<Compiler>);
static_assert(std::is_enum_v<Architecture>);
static_assert(noexcept(platform()));
static_assert(noexcept(compiler()));
static_assert(noexcept(architecture()));
static_assert(noexcept(is_debug()));
static_assert(noexcept(is_release()));
static_assert(noexcept(platform_name()));
static_assert(noexcept(compiler_name()));
static_assert(noexcept(architecture_name()));
static_assert(noexcept(cpp_standard()));
static_assert(noexcept(pointer_size()));
static_assert(noexcept(native_endian()));

} // namespace nexus::core::test
