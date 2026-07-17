// Unit tests for nexus::core::text string utilities (Phase 8F).

#include "nexus/core/text/strings.hpp"

#include <gtest/gtest.h>
#include <limits>

namespace nexus::core::text::test {

// ------------------------------------------
// Trim
// ------------------------------------------

TEST(TrimTest, LeftRemovesLeadingWhitespaceOnly)
{
    EXPECT_EQ(trim_left("  \t hello  "), "hello  ");
}

TEST(TrimTest, RightRemovesTrailingWhitespaceOnly)
{
    EXPECT_EQ(trim_right("  hello \t "), "  hello");
}

TEST(TrimTest, TrimRemovesBothSides)
{
    EXPECT_EQ(trim(" \n hello \r\n "), "hello");
}

TEST(TrimTest, EmptyStringYieldsEmptyString)
{
    EXPECT_EQ(trim(""), "");
}

TEST(TrimTest, AllWhitespaceYieldsEmptyString)
{
    EXPECT_EQ(trim("   \t\n\r\v\f  "), "");
}

TEST(TrimTest, NoWhitespaceIsUnchanged)
{
    EXPECT_EQ(trim("hello"), "hello");
}

// ------------------------------------------
// Split / Join
// ------------------------------------------

TEST(SplitTest, BasicDelimiter)
{
    const auto parts = split("a,b,c", ",");
    ASSERT_EQ(parts.size(), 3U);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
}

TEST(SplitTest, EmptyViewYieldsSingleEmptyElement)
{
    const auto parts = split("", ",");
    ASSERT_EQ(parts.size(), 1U);
    EXPECT_EQ(parts[0], "");
}

TEST(SplitTest, ConsecutiveDelimitersYieldEmptyElements)
{
    const auto parts = split("a,,b", ",");
    ASSERT_EQ(parts.size(), 3U);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "");
    EXPECT_EQ(parts[2], "b");
}

TEST(SplitTest, EmptyDelimiterYieldsWholeViewUnchanged)
{
    const auto parts = split("abc", "");
    ASSERT_EQ(parts.size(), 1U);
    EXPECT_EQ(parts[0], "abc");
}

TEST(SplitTest, MultiCharacterDelimiter)
{
    const auto parts = split("a::b::c", "::");
    ASSERT_EQ(parts.size(), 3U);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
}

TEST(SplitAnyTest, SplitsOnAnyDelimiterCharacter)
{
    const auto parts = split_any("a,b;c d", ",; ");
    ASSERT_EQ(parts.size(), 4U);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
    EXPECT_EQ(parts[3], "d");
}

TEST(JoinTest, JoinsPartsWithDelimiter)
{
    const std::vector<StringView> parts{"a", "b", "c"};
    EXPECT_EQ(join(parts, ", "), "a, b, c");
}

TEST(JoinTest, EmptyPartsYieldsEmptyString)
{
    const std::vector<StringView> parts{};
    EXPECT_EQ(join(parts, ", "), "");
}

TEST(JoinTest, SinglePartYieldsPartUnchanged)
{
    const std::vector<StringView> parts{"solo"};
    EXPECT_EQ(join(parts, ", "), "solo");
}

TEST(SplitJoinTest, RoundTripsWithMatchingDelimiter)
{
    const StringView original = "one|two|three";
    const auto parts = split(original, "|");
    EXPECT_EQ(join(parts, "|"), original);
}

// ------------------------------------------
// Replace
// ------------------------------------------

TEST(ReplaceAllTest, ReplacesEveryOccurrence)
{
    EXPECT_EQ(replace_all("foo bar foo", "foo", "baz"), "baz bar baz");
}

TEST(ReplaceAllTest, EmptyTargetLeavesViewUnchanged)
{
    EXPECT_EQ(replace_all("hello", "", "x"), "hello");
}

TEST(ReplaceAllTest, TargetNotFoundLeavesViewUnchanged)
{
    EXPECT_EQ(replace_all("hello", "xyz", "abc"), "hello");
}

TEST(ReplaceAllTest, EmptyViewYieldsEmptyString)
{
    EXPECT_EQ(replace_all("", "foo", "bar"), "");
}

TEST(ReplaceFirstTest, ReplacesOnlyFirstOccurrence)
{
    EXPECT_EQ(replace_first("foo bar foo", "foo", "baz"), "baz bar foo");
}

TEST(ReplaceFirstTest, TargetNotFoundLeavesViewUnchanged)
{
    EXPECT_EQ(replace_first("hello", "xyz", "abc"), "hello");
}

// ------------------------------------------
// Prefix / Suffix
// ------------------------------------------

TEST(StartsWithTest, DetectsMatchingPrefix)
{
    EXPECT_TRUE(starts_with("hello world", "hello"));
    EXPECT_FALSE(starts_with("hello world", "world"));
}

TEST(StartsWithTest, EmptyPrefixAlwaysMatches)
{
    EXPECT_TRUE(starts_with("hello", ""));
}

TEST(StartsWithTest, PrefixLongerThanViewFails)
{
    EXPECT_FALSE(starts_with("hi", "hello"));
}

TEST(EndsWithTest, DetectsMatchingSuffix)
{
    EXPECT_TRUE(ends_with("hello world", "world"));
    EXPECT_FALSE(ends_with("hello world", "hello"));
}

TEST(EndsWithTest, SuffixLongerThanViewFails)
{
    EXPECT_FALSE(ends_with("hi", "hello"));
}

TEST(ContainsTest, DetectsSubstringAnywhere)
{
    EXPECT_TRUE(contains("hello world", "lo wo"));
    EXPECT_FALSE(contains("hello world", "xyz"));
}

TEST(ContainsTest, EmptyNeedleAlwaysMatches)
{
    EXPECT_TRUE(contains("hello", ""));
}

TEST(RemovePrefixTest, RemovesMatchingPrefix)
{
    EXPECT_EQ(remove_prefix("hello world", "hello "), "world");
}

TEST(RemovePrefixTest, NonMatchingPrefixLeavesViewUnchanged)
{
    EXPECT_EQ(remove_prefix("hello world", "world"), "hello world");
}

TEST(RemoveSuffixTest, RemovesMatchingSuffix)
{
    EXPECT_EQ(remove_suffix("hello world", " world"), "hello");
}

TEST(RemoveSuffixTest, NonMatchingSuffixLeavesViewUnchanged)
{
    EXPECT_EQ(remove_suffix("hello world", "hello"), "hello world");
}

// ------------------------------------------
// ASCII case conversion
// ------------------------------------------

TEST(AsciiCaseTest, ToLowerConvertsUppercaseOnly)
{
    EXPECT_EQ(to_lower_ascii("Hello, World! 123"), "hello, world! 123");
}

TEST(AsciiCaseTest, ToUpperConvertsLowercaseOnly)
{
    EXPECT_EQ(to_upper_ascii("Hello, World! 123"), "HELLO, WORLD! 123");
}

TEST(AsciiCaseTest, NonAsciiBytesPassThroughUnchanged)
{
    // 0xC3 0xA9 is UTF-8 for 'e' with acute accent; must not be touched
    // by ASCII-only case conversion.
    const std::string input = "caf\xC3\xA9";
    EXPECT_EQ(to_upper_ascii(input), "CAF\xC3\xA9");
}

TEST(AsciiCaseTest, EmptyStringYieldsEmptyString)
{
    EXPECT_EQ(to_lower_ascii(""), "");
    EXPECT_EQ(to_upper_ascii(""), "");
}

TEST(EqualsIgnoreCaseAsciiTest, MatchesRegardlessOfAsciiCase)
{
    EXPECT_TRUE(equals_ignore_case_ascii("Hello", "HELLO"));
    EXPECT_TRUE(equals_ignore_case_ascii("Hello", "hello"));
}

TEST(EqualsIgnoreCaseAsciiTest, DifferentLengthsNeverMatch)
{
    EXPECT_FALSE(equals_ignore_case_ascii("Hello", "Hello!"));
}

TEST(EqualsIgnoreCaseAsciiTest, DifferentContentDoesNotMatch)
{
    EXPECT_FALSE(equals_ignore_case_ascii("Hello", "World"));
}

// ------------------------------------------
// Numeric conversion
// ------------------------------------------

TEST(ParseNumberTest, ParsesValidInteger)
{
    const auto result = parse_number<int>("42");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

TEST(ParseNumberTest, ParsesNegativeInteger)
{
    const auto result = parse_number<int>("-42");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, -42);
}

TEST(ParseNumberTest, InvalidArgumentFailsWithInvalidArgument)
{
    const auto result = parse_number<int>("not_a_number");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::InvalidArgument);
}

TEST(ParseNumberTest, EmptyStringFailsWithInvalidArgument)
{
    const auto result = parse_number<int>("");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::InvalidArgument);
}

TEST(ParseNumberTest, OutOfRangeFailsWithOutOfRange)
{
    const auto result = parse_number<std::int8_t>("1000");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::OutOfRange);
}

TEST(ParseNumberTest, TrailingCharactersFail)
{
    const auto result = parse_number<int>("42abc");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::TrailingCharacters);
}

TEST(FormatNumberTest, FormatsPositiveInteger)
{
    EXPECT_EQ(format_number(42), "42");
}

TEST(FormatNumberTest, FormatsNegativeInteger)
{
    EXPECT_EQ(format_number(-42), "-42");
}

TEST(FormatNumberTest, FormatsZero)
{
    EXPECT_EQ(format_number(0), "0");
}

TEST(FormatNumberTest, FormatsMaxInt64)
{
    EXPECT_EQ(format_number(std::numeric_limits<std::int64_t>::max()), "9223372036854775807");
}

TEST(NumericRoundTripTest, ParseAndFormatAreInverses)
{
    const auto parsed = parse_number<int>("12345");
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(format_number(*parsed), "12345");
}

// ------------------------------------------
// Hexadecimal formatting / parsing
// ------------------------------------------

TEST(ToHexTest, FormatsWithFixedWidthAndPadding)
{
    EXPECT_EQ(to_hex(static_cast<std::uint32_t>(0xAB)), "000000ab");
}

TEST(ToHexTest, FormatsMaxValueWithNoPadding)
{
    EXPECT_EQ(to_hex(static_cast<std::uint8_t>(0xFF)), "ff");
}

TEST(ToHexTest, FormatsZero)
{
    EXPECT_EQ(to_hex(static_cast<std::uint16_t>(0)), "0000");
}

TEST(FromHexTest, ParsesLowercaseDigits)
{
    const auto result = from_hex<std::uint32_t>("000000ab");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0xABU);
}

TEST(FromHexTest, ParsesWithLowercase0xPrefix)
{
    const auto result = from_hex<std::uint32_t>("0xab");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0xABU);
}

TEST(FromHexTest, ParsesWithUppercase0XPrefix)
{
    const auto result = from_hex<std::uint32_t>("0XAB");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0xABU);
}

TEST(FromHexTest, EmptyStringFailsWithInvalidArgument)
{
    const auto result = from_hex<std::uint32_t>("");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::InvalidArgument);
}

TEST(FromHexTest, PrefixOnlyFailsWithInvalidArgument)
{
    const auto result = from_hex<std::uint32_t>("0x");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::InvalidArgument);
}

TEST(FromHexTest, InvalidDigitsFailWithInvalidArgument)
{
    const auto result = from_hex<std::uint32_t>("zzzz");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::InvalidArgument);
}

TEST(FromHexTest, OutOfRangeFailsWithOutOfRange)
{
    const auto result = from_hex<std::uint8_t>("ffff");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ConversionError::OutOfRange);
}

TEST(HexRoundTripTest, ToHexAndFromHexAreInverses)
{
    constexpr std::uint32_t original = 0xDEADBEEF;
    const auto formatted = to_hex(original);
    const auto parsed = from_hex<std::uint32_t>(formatted);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(*parsed, original);
}

// ------------------------------------------
// StringBuilder
// ------------------------------------------

TEST(StringBuilderTest, AppendsTextInOrder)
{
    StringBuilder builder;
    builder.append("hello").append(' ').append("world");
    EXPECT_EQ(std::move(builder).build(), "hello world");
}

TEST(StringBuilderTest, StartsEmpty)
{
    const StringBuilder builder;
    EXPECT_TRUE(builder.empty());
    EXPECT_EQ(builder.size(), 0U);
}

TEST(StringBuilderTest, SizeReflectsAppendedContent)
{
    StringBuilder builder;
    builder.append("abc");
    EXPECT_EQ(builder.size(), 3U);
    EXPECT_FALSE(builder.empty());
}

TEST(StringBuilderTest, AppendNumberAppendsFormattedValue)
{
    StringBuilder builder;
    builder.append("count=").append_number(42);
    EXPECT_EQ(std::move(builder).build(), "count=42");
}

TEST(StringBuilderTest, ClearResetsContentButKeepsUsable)
{
    StringBuilder builder;
    builder.append("hello");
    builder.clear();
    EXPECT_TRUE(builder.empty());
    builder.append("world");
    EXPECT_EQ(std::move(builder).build(), "world");
}

TEST(StringBuilderTest, ViewReflectsCurrentContentsWithoutConsuming)
{
    StringBuilder builder;
    builder.append("hello");
    EXPECT_EQ(builder.view(), "hello");
    // Builder is still usable after view().
    builder.append(" world");
    EXPECT_EQ(builder.view(), "hello world");
}

TEST(StringBuilderTest, ConstBuildCopiesWithoutConsuming)
{
    StringBuilder builder;
    builder.append("hello");
    const String first = builder.build();
    const String second = builder.build();
    EXPECT_EQ(first, "hello");
    EXPECT_EQ(second, "hello");
}

TEST(StringBuilderTest, ReserveDoesNotAffectContent)
{
    StringBuilder builder;
    builder.reserve(1024);
    EXPECT_TRUE(builder.empty());
    builder.append("ok");
    EXPECT_EQ(std::move(builder).build(), "ok");
}

TEST(StringBuilderTest, ConstructorReservesCapacityWithoutAddingContent)
{
    const StringBuilder builder(64);
    EXPECT_TRUE(builder.empty());
    EXPECT_EQ(builder.size(), 0U);
}

TEST(StringBuilderTest, HandlesLargeAppendedContent)
{
    StringBuilder builder;
    const std::string chunk(10'000, 'x');
    for (int i = 0; i < 10; ++i) {
        builder.append(chunk);
    }
    EXPECT_EQ(std::move(builder).build().size(), 100'000U);
}

} // namespace nexus::core::text::test
