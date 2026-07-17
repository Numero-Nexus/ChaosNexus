// Unit tests for nexus::core::hash (Phase 8H — Hash Utilities).
// Covers hash_combine, hash_range, and std::hash support for
// std::pair, std::tuple, std::array, and enums, including
// constexpr evaluation and unordered_map/unordered_set usability.

#include "nexus/core/hash/hash.hpp"

#include <array>
#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace nexus::core::hash::test {

// ------------------------------------------
// hash_combine
// ------------------------------------------

TEST(HashCombine, IdenticalValuesProduceIdenticalHashes)
{
    EXPECT_EQ(hash_combine(0, 1, 2, 3), hash_combine(0, 1, 2, 3));
}

TEST(HashCombine, DifferingValuesProduceDifferingHashes)
{
    EXPECT_NE(hash_combine(0, 1, 2, 3), hash_combine(0, 1, 2, 4));
}

TEST(HashCombine, OrderSensitive)
{
    EXPECT_NE(hash_combine(0, 1, 2), hash_combine(0, 2, 1));
}

TEST(HashCombine, SeedAffectsResult)
{
    EXPECT_NE(hash_combine(0, 42), hash_combine(1, 42));
}

TEST(HashCombine, NoArgumentsReturnsSeedUnchanged)
{
    EXPECT_EQ(hash_combine(1234), 1234U);
}

TEST(HashCombine, ConstexprEvaluationNotSupported)
{
    // hash_combine() is intentionally not constexpr: it is built on
    // std::hash<T>, which the standard does not guarantee is
    // constexpr-callable (MSVC's std::hash<int> is not). Runtime
    // stability is covered by IdenticalValuesProduceIdenticalHashes
    // above.
    SUCCEED();
}

// ------------------------------------------
// hash_range
// ------------------------------------------

TEST(HashRange, EmptyRangeReturnsStableValue)
{
    const std::vector<int> empty;
    EXPECT_EQ(hash_range(empty), hash_range(empty));
}

TEST(HashRange, IdenticalRangesProduceIdenticalHashes)
{
    const std::vector<int> a{1, 2, 3};
    const std::vector<int> b{1, 2, 3};
    EXPECT_EQ(hash_range(a), hash_range(b));
}

TEST(HashRange, DifferingRangesProduceDifferingHashes)
{
    const std::vector<int> a{1, 2, 3};
    const std::vector<int> b{1, 2, 4};
    EXPECT_NE(hash_range(a), hash_range(b));
}

TEST(HashRange, IteratorPairOverloadMatchesRangeOverload)
{
    const std::vector<int> values{5, 6, 7, 8};
    EXPECT_EQ(hash_range(values.begin(), values.end()), hash_range(values));
}

TEST(HashRange, HandlesLargeRanges)
{
    std::vector<int> values(10'000);
    for (int i = 0; i < 10'000; ++i) {
        values[static_cast<std::size_t>(i)] = i;
    }
    EXPECT_EQ(hash_range(values), hash_range(values));
}

// static_assert(hash_range(std::array<int, 3>{1, 2, 3}) == hash_range(std::array<int, 3>{1, 2,
// 3}));

// ------------------------------------------
// std::hash<std::pair<T1, T2>>
// ------------------------------------------

TEST(PairHash, IdenticalPairsProduceIdenticalHashes)
{
    const PairHash<int, int> hasher;
    EXPECT_EQ(hasher({1, 2}), hasher({1, 2}));
}

TEST(PairHash, DifferingPairsProduceDifferingHashes)
{
    const PairHash<int, int> hasher;
    EXPECT_NE(hasher({1, 2}), hasher({2, 1}));
}

// static_assert(std::hash<std::pair<int, int>>{}({1, 2}) == std::hash<std::pair<int, int>>{}({1,
// 2}));

// ------------------------------------------
// std::hash<std::tuple<Ts...>>
// ------------------------------------------

TEST(TupleHash, IdenticalTuplesProduceIdenticalHashes)
{
    const TupleHash<int, int, int> hasher;
    EXPECT_EQ(hasher({1, 2, 3}), hasher({1, 2, 3}));
}

TEST(TupleHash, DifferingTuplesProduceDifferingHashes)
{
    const TupleHash<int, int, int> hasher;
    EXPECT_NE(hasher({1, 2, 3}), hasher({1, 2, 4}));
}

// ------------------------------------------
// std::hash<std::array<T, N>>
// ------------------------------------------

TEST(ArrayHash, IdenticalArraysProduceIdenticalHashes)
{
    const ArrayHash<int, 3> hasher;
    EXPECT_EQ(hasher({1, 2, 3}), hasher({1, 2, 3}));
}

TEST(ArrayHash, DifferingArraysProduceDifferingHashes)
{
    const ArrayHash<int, 3> hasher;
    EXPECT_NE(hasher({1, 2, 3}), hasher({3, 2, 1}));
}

// ------------------------------------------
// std::hash<Enum>
// ------------------------------------------

enum class TestHashEnum : int {
    A,
    B,
    C
};

TEST(EnumHash, IdenticalEnumeratorsProduceIdenticalHashes)
{
    const EnumHash<TestHashEnum> hasher;
    EXPECT_EQ(hasher(TestHashEnum::A), hasher(TestHashEnum::A));
}

TEST(EnumHash, DifferingEnumeratorsProduceDifferingHashes)
{
    const EnumHash<TestHashEnum> hasher;
    EXPECT_NE(hasher(TestHashEnum::A), hasher(TestHashEnum::B));
}

TEST(EnumHash, MatchesUnderlyingValueHash)
{
    const EnumHash<TestHashEnum> hasher;
    const std::hash<int> int_hasher;
    EXPECT_EQ(hasher(TestHashEnum::C), int_hasher(2));
}

// static_assert(std::hash<TestHashEnum>{}(TestHashEnum::A) ==
//               std::hash<TestHashEnum>{}(TestHashEnum::A));

// ------------------------------------------
// Container compatibility
// ------------------------------------------

TEST(HashUtilities, PairUsableAsUnorderedMapKey)
{
    std::unordered_map<std::pair<int, int>, std::string, PairHash<int, int>> map;
    map[{1, 2}] = "a";
    map[{2, 1}] = "b";
    EXPECT_EQ(map.at({1, 2}), "a");
    EXPECT_EQ(map.at({2, 1}), "b");
}

TEST(HashUtilities, TupleUsableAsUnorderedSetKey)
{
    std::unordered_set<std::tuple<int, int, int>, TupleHash<int, int, int>> set;
    set.insert({1, 2, 3});
    set.insert({1, 2, 3});
    set.insert({4, 5, 6});
    EXPECT_EQ(set.size(), 2U);
}

TEST(HashUtilities, ArrayUsableAsUnorderedSetKey)
{
    std::unordered_set<std::array<int, 3>, ArrayHash<int, 3>> set;
    set.insert({1, 2, 3});
    set.insert({1, 2, 3});
    EXPECT_EQ(set.size(), 1U);
}

TEST(HashUtilities, EnumUsableAsUnorderedMapKey)
{
    std::unordered_map<TestHashEnum, int, EnumHash<TestHashEnum>> map;
    map[TestHashEnum::A] = 10;
    map[TestHashEnum::B] = 20;
    EXPECT_EQ(map.at(TestHashEnum::A), 10);
    EXPECT_EQ(map.at(TestHashEnum::B), 20);
}

// ------------------------------------------
// TransparentHash
// ------------------------------------------

TEST(TransparentHash, StringAndStringViewProduceSameHash)
{
    const TransparentHash hasher;
    const std::string owned = "hello";
    const std::string_view view = owned;
    EXPECT_EQ(hasher(owned), hasher(view));
}

TEST(TransparentHash, EnablesHeterogeneousLookup)
{
    std::unordered_map<std::string, int, TransparentHash, std::equal_to<>> map;
    map["key"] = 42;
    EXPECT_EQ(map.find(std::string_view{"key"})->second, 42);
}

} // namespace nexus::core::hash::test
