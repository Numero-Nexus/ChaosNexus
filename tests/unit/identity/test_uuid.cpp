// Unit tests for nexus::core Uuid (Phase 8E — UUID & Strong
// Identifier Framework). Covers construction, nil checks, equality,
// ordering, parsing, formatting, invalid input, hashing, and
// std::format integration.

#include "nexus/core/identity/uuid.hpp"
#include "nexus/core/identity/uuid_format.hpp"
#include "nexus/core/identity/uuid_hash.hpp"

#include <format>
#include <gtest/gtest.h>
#include <string>
#include <unordered_set>

namespace nexus::core::test {

namespace {

[[nodiscard]] auto make_uuid(Uuid::ByteArray bytes) -> Uuid
{
    return Uuid{bytes};
}

} // namespace

TEST(Uuid, DefaultConstructedIsNil)
{
    constexpr Uuid id;
    EXPECT_TRUE(id.is_nil());
}

TEST(Uuid, NilFactoryIsNil)
{
    EXPECT_TRUE(Uuid::nil().is_nil());
}

TEST(Uuid, NonZeroBytesAreNotNil)
{
    const Uuid id = make_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
    EXPECT_FALSE(id.is_nil());
}

TEST(Uuid, EqualityComparesByteValue)
{
    const Uuid a = make_uuid({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    const Uuid b = make_uuid({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    const Uuid c = make_uuid({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17});

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(Uuid, OrderingIsLexicographicByBytes)
{
    const Uuid lower = make_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
    const Uuid higher = make_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2});

    EXPECT_LT(lower, higher);
    EXPECT_GT(higher, lower);
    EXPECT_LE(lower, lower);
}

TEST(Uuid, IsConstexprEvaluable)
{
    static_assert(Uuid::nil().is_nil());
    static_assert(Uuid{} == Uuid::nil());
}

TEST(UuidFormat, ToStringProducesCanonicalLowercaseForm)
{
    const Uuid id = make_uuid({0x55, 0x0e, 0x84, 0x00, 0xe2, 0x9b, 0x41, 0xd4, 0xa7, 0x16, 0x44,
                               0x66, 0x55, 0x44, 0x00, 0x00});

    EXPECT_EQ(to_string(id), "550e8400-e29b-41d4-a716-446655440000");
}

TEST(UuidFormat, NilUuidFormatsAsAllZeroes)
{
    EXPECT_EQ(to_string(Uuid::nil()), "00000000-0000-0000-0000-000000000000");
}

TEST(UuidFormat, RoundTripsThroughStringConversion)
{
    const Uuid original = make_uuid({0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xfe, 0xdc,
                                     0xba, 0x98, 0x76, 0x54, 0x32, 0x10});

    const std::string text = to_string(original);
    const Result<Uuid> parsed = uuid_from_string(text);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed.value(), original);
}

TEST(UuidFormat, FromStringAcceptsUppercaseHex)
{
    const Result<Uuid> parsed = uuid_from_string("550E8400-E29B-41D4-A716-446655440000");

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(to_string(parsed.value()), "550e8400-e29b-41d4-a716-446655440000");
}

TEST(UuidFormat, FromStringRejectsWrongLength)
{
    const Result<Uuid> parsed = uuid_from_string("550e8400-e29b-41d4-a716-44665544000");

    EXPECT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error().code(), ErrorCode::ConfigParseFailed);
}

TEST(UuidFormat, FromStringRejectsMissingHyphens)
{
    const Result<Uuid> parsed = uuid_from_string("550e8400e29b41d4a716446655440000xx");

    EXPECT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error().code(), ErrorCode::ConfigParseFailed);
}

TEST(UuidFormat, FromStringRejectsNonHexCharacters)
{
    const Result<Uuid> parsed = uuid_from_string("550e8400-e29b-41d4-a716-44665544zzzz");

    EXPECT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error().code(), ErrorCode::ConfigParseFailed);
}

TEST(UuidFormat, StdFormatUsesCanonicalRepresentation)
{
    const Uuid id = make_uuid({0x55, 0x0e, 0x84, 0x00, 0xe2, 0x9b, 0x41, 0xd4, 0xa7, 0x16, 0x44,
                               0x66, 0x55, 0x44, 0x00, 0x00});

    EXPECT_EQ(std::format("id={}", id), "id=550e8400-e29b-41d4-a716-446655440000");
}

TEST(UuidHash, EqualUuidsHashEqual)
{
    const Uuid a = make_uuid({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    const Uuid b = make_uuid({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});

    EXPECT_EQ(std::hash<Uuid>{}(a), std::hash<Uuid>{}(b));
}

TEST(UuidHash, UsableAsUnorderedSetKey)
{
    std::unordered_set<Uuid> ids;
    ids.insert(make_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    ids.insert(make_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}));
    ids.insert(make_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1})); // duplicate

    EXPECT_EQ(ids.size(), 2U);
}

} // namespace nexus::core::test
