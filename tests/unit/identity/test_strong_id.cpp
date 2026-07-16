// Unit tests for nexus::core StrongId<Tag> and the framework's
// initial concrete identifiers (Phase 8E). Covers type safety,
// comparison, hashing, formatting, and the id_helpers.hpp free
// functions.

#include "nexus/core/identity/id_helpers.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/identity/strong_id.hpp"
#include "nexus/core/identity/uuid.hpp"

#include <format>
#include <gtest/gtest.h>
#include <type_traits>
#include <unordered_set>

namespace nexus::core::test {

namespace {

struct TagA {};
struct TagB {};

// Local helper producing a fixed, non-nil Uuid for tests in this
// translation unit. Kept file-local rather than added to
// nexus-test-common (tests/common/) since it's a single arbitrary
// byte pattern only needed here — promoting it to shared test
// infrastructure isn't warranted for one call site's worth of use.
[[nodiscard]] auto make_uuid_for_test() -> Uuid
{
    return Uuid{Uuid::ByteArray{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x11, 0x22, 0x33,
                                0x44, 0x55, 0x66, 0x77, 0x88}};
}

} // namespace

TEST(StrongId, DefaultConstructedIsNil)
{
    const StrongId<TagA> id;
    EXPECT_TRUE(id.is_nil());
}

TEST(StrongId, WrapsProvidedUuidValue)
{
    const Uuid raw = make_uuid_for_test();
    const StrongId<TagA> id{raw};

    EXPECT_EQ(id.value(), raw);
    EXPECT_FALSE(id.is_nil());
}

TEST(StrongId, DistinctTagsAreDistinctTypes)
{
    static_assert(!std::is_same_v<StrongId<TagA>, StrongId<TagB>>);
    static_assert(!std::is_convertible_v<StrongId<TagA>, StrongId<TagB>>);
}

TEST(StrongId, EqualityAndOrderingMatchUnderlyingUuid)
{
    const Uuid raw = make_uuid_for_test();
    const StrongId<TagA> a{raw};
    const StrongId<TagA> b{raw};

    EXPECT_EQ(a, b);
    EXPECT_FALSE(a < b);
}

TEST(StrongId, ZeroOverheadSameSizeAsUuid)
{
    static_assert(sizeof(StrongId<TagA>) == sizeof(Uuid));
    static_assert(std::is_trivially_copyable_v<StrongId<TagA>>);
}

TEST(StrongId, ToStringMatchesUnderlyingUuid)
{
    const Uuid raw = make_uuid_for_test();
    const StrongId<TagA> id{raw};

    EXPECT_EQ(to_string(id), to_string(raw));
}

TEST(StrongId, StdFormatUsesToString)
{
    const Uuid raw = make_uuid_for_test();
    const StrongId<TagA> id{raw};

    EXPECT_EQ(std::format("{}", id), to_string(raw));
}

TEST(StrongId, HashableAndUsableAsUnorderedSetKey)
{
    std::unordered_set<StrongId<TagA>> ids;
    ids.insert(StrongId<TagA>{make_uuid_for_test()});
    ids.insert(StrongId<TagA>::nil());

    EXPECT_EQ(ids.size(), 2U);
}

TEST(StrongIdHelpers, IsNilFreeFunctionMatchesMember)
{
    EXPECT_TRUE(is_nil(StrongId<TagA>::nil()));
    EXPECT_FALSE(is_nil(StrongId<TagA>{make_uuid_for_test()}));
}

TEST(StrongIdHelpers, ValueFreeFunctionMatchesMember)
{
    const Uuid raw = make_uuid_for_test();
    const StrongId<TagA> id{raw};

    EXPECT_EQ(value(id), raw);
}

TEST(StrongIdHelpers, FromStringParsesValidUuidText)
{
    const Result<StrongId<TagA>> parsed = from_string<TagA>("550e8400-e29b-41d4-a716-446655440000");

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(to_string(parsed.value()), "550e8400-e29b-41d4-a716-446655440000");
}

TEST(StrongIdHelpers, FromStringForwardsParseFailure)
{
    const Result<StrongId<TagA>> parsed = from_string<TagA>("not-a-uuid");

    EXPECT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error().code(), ErrorCode::ConfigParseFailed);
}

TEST(FrameworkIds, NodeEventAndSimulationIdAreDistinctTypes)
{
    static_assert(!std::is_same_v<NodeId, EventId>);
    static_assert(!std::is_same_v<EventId, SimulationId>);
    static_assert(!std::is_same_v<NodeId, SimulationId>);
}

TEST(FrameworkIds, DefaultConstructAndCompareAsExpected)
{
    const NodeId node_id{};
    const EventId event_id{};

    EXPECT_TRUE(node_id.is_nil());
    EXPECT_TRUE(event_id.is_nil());
}

} // namespace nexus::core::test
