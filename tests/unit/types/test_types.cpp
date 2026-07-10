// Unit tests for nexus::core::types (Phase 8B — Core Types & Common
// Definitions). Covers integer/float/byte/size aliases and the
// concepts defined in concepts.hpp, primarily via static_assert
// compile-time verification, with a small number of runtime checks
// where a compile-time check alone would not exercise behavior.

#include "nexus/core/types/concepts.hpp"
#include "nexus/core/types/types.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

namespace nexus::core::types::test
{

// ------------------------------------------
// Fixed-width integer aliases
// ------------------------------------------

static_assert(std::is_same_v<I8, std::int8_t>);
static_assert(std::is_same_v<I16, std::int16_t>);
static_assert(std::is_same_v<I32, std::int32_t>);
static_assert(std::is_same_v<I64, std::int64_t>);

static_assert(std::is_same_v<U8, std::uint8_t>);
static_assert(std::is_same_v<U16, std::uint16_t>);
static_assert(std::is_same_v<U32, std::uint32_t>);
static_assert(std::is_same_v<U64, std::uint64_t>);

static_assert(sizeof(I8) == 1 && sizeof(U8) == 1);
static_assert(sizeof(I16) == 2 && sizeof(U16) == 2);
static_assert(sizeof(I32) == 4 && sizeof(U32) == 4);
static_assert(sizeof(I64) == 8 && sizeof(U64) == 8);

static_assert(std::is_signed_v<I8> && std::is_signed_v<I64>);
static_assert(std::is_unsigned_v<U8> && std::is_unsigned_v<U64>);

TEST(TypesAliases, IntegerRangesMatchWidth)
{
    EXPECT_EQ(static_cast<I8>(-1), I8{-1});
    EXPECT_EQ(static_cast<U8>(255), U8{255});
    EXPECT_EQ(static_cast<U64>(0xFFFFFFFFFFFFFFFFULL), U64{0xFFFFFFFFFFFFFFFFULL});
}

// ------------------------------------------
// Floating-point aliases
// ------------------------------------------

static_assert(std::is_same_v<F32, float>);
static_assert(std::is_same_v<F64, double>);
static_assert(sizeof(F32) == 4);
static_assert(sizeof(F64) == 8);

TEST(TypesAliases, FloatingPointArithmetic)
{
    constexpr F32 a = 1.5F;
    constexpr F64 b = 2.5;
    EXPECT_FLOAT_EQ(a + a, 3.0F);
    EXPECT_DOUBLE_EQ(b + b, 5.0);
}

// ------------------------------------------
// Byte aliases
// ------------------------------------------

static_assert(std::is_same_v<Byte, std::byte>);
static_assert(std::is_same_v<RawByte, U8>);
static_assert(sizeof(Byte) == 1);
static_assert(sizeof(RawByte) == 1);

TEST(TypesAliases, ByteConstructionAndConversion)
{
    constexpr Byte b{0xAB};
    EXPECT_EQ(std::to_integer<RawByte>(b), RawByte{0xAB});

    constexpr RawByte raw = 0x7F;
    EXPECT_EQ(raw, RawByte{0x7F});
}

// ------------------------------------------
// Size and index aliases
// ------------------------------------------

static_assert(std::is_same_v<Size, std::size_t>);
static_assert(std::is_same_v<Diff, std::ptrdiff_t>);
static_assert(std::is_same_v<Index, std::size_t>);
static_assert(std::is_unsigned_v<Size>);
static_assert(std::is_signed_v<Diff>);
static_assert(std::is_unsigned_v<Index>);

// ------------------------------------------
// UOfSize / IOfSize helper traits
// ------------------------------------------

static_assert(std::is_same_v<UOfSize<1>, U8>);
static_assert(std::is_same_v<UOfSize<2>, U16>);
static_assert(std::is_same_v<UOfSize<4>, U32>);
static_assert(std::is_same_v<UOfSize<8>, U64>);

static_assert(std::is_same_v<IOfSize<1>, I8>);
static_assert(std::is_same_v<IOfSize<2>, I16>);
static_assert(std::is_same_v<IOfSize<4>, I32>);
static_assert(std::is_same_v<IOfSize<8>, I64>);

// ------------------------------------------
// Concepts: SignedIntegral / UnsignedIntegral / Integral
// ------------------------------------------

static_assert(SignedIntegral<I8>);
static_assert(SignedIntegral<I64>);
static_assert(!SignedIntegral<U32>);
static_assert(!SignedIntegral<bool>);

static_assert(UnsignedIntegral<U8>);
static_assert(UnsignedIntegral<Size>);
static_assert(!UnsignedIntegral<I32>);
static_assert(!UnsignedIntegral<bool>);

static_assert(Integral<I32>);
static_assert(Integral<U64>);
static_assert(!Integral<bool>);
static_assert(!Integral<F32>);

// ------------------------------------------
// Concepts: FloatingPoint / Arithmetic
// ------------------------------------------

static_assert(FloatingPoint<F32>);
static_assert(FloatingPoint<F64>);
static_assert(!FloatingPoint<I32>);

static_assert(Arithmetic<I32>);
static_assert(Arithmetic<U64>);
static_assert(Arithmetic<F32>);
static_assert(!Arithmetic<bool>);

// ------------------------------------------
// Concepts: Enum / ScopedEnum
// ------------------------------------------

enum class TestScopedEnum : U8
{
    A,
    B
};

enum TestUnscopedEnum
{
    X,
    Y
};

static_assert(Enum<TestScopedEnum>);
static_assert(Enum<TestUnscopedEnum>);
static_assert(!Enum<I32>);

static_assert(ScopedEnum<TestScopedEnum>);
static_assert(!ScopedEnum<TestUnscopedEnum>);
static_assert(!ScopedEnum<I32>);

// ------------------------------------------
// Concepts: TriviallyCopyable / TriviallyLayoutCompatible
// ------------------------------------------

struct TriviallyCopyableStruct
{
    I32 a;
    F64 b;
};

struct NonTrivialStruct
{
    NonTrivialStruct() = default;
    ~NonTrivialStruct() { }
    I32 a{};
};

static_assert(TriviallyCopyable<TriviallyCopyableStruct>);
static_assert(!TriviallyCopyable<NonTrivialStruct>);

static_assert(TriviallyLayoutCompatible<TriviallyCopyableStruct>);
static_assert(!TriviallyLayoutCompatible<NonTrivialStruct>);

// ------------------------------------------
// Template constraint verification (functions using the concepts)
// ------------------------------------------

template <Arithmetic T>
constexpr auto double_value(T value) noexcept -> T
{
    return value + value;
}

TEST(TypesConcepts, ConstrainedTemplateAcceptsArithmeticTypes)
{
    EXPECT_EQ(double_value(I32{21}), I32{42});
    EXPECT_DOUBLE_EQ(double_value(F64{2.5}), 5.0);
}

} // namespace nexus::core::types::test
