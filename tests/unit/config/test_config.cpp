// Unit tests for nexus::core Configuration Primitives (Phase 8G).
// Covers only the essential contract of ConfigurationKey and
// ConfigurationValue: construction/type identification, one
// success and one failure path per accessor, equality, and string
// conversion. Exhaustive per-type/per-edge-case coverage is
// deliberately deferred to whichever future phase consumes these
// primitives in a way that would actually exercise it.

#include "nexus/core/config/config_key.hpp"
#include "nexus/core/config/config_value.hpp"

#include <gtest/gtest.h>
#include <string>

namespace nexus::core::test {

TEST(ConfigurationKeyTest, StoresAndReturnsName)
{
    const ConfigurationKey key{"nexus.log_level"};
    EXPECT_EQ(key.name(), "nexus.log_level");
}

TEST(ConfigurationKeyTest, EqualityAndOrderingAreValueBased)
{
    EXPECT_EQ(ConfigurationKey{"a"}, ConfigurationKey{"a"});
    EXPECT_NE(ConfigurationKey{"a"}, ConfigurationKey{"b"});
    EXPECT_LT(ConfigurationKey{"a"}, ConfigurationKey{"b"});
}

TEST(ConfigurationKeyTest, IsValidNameRejectsEmptyAndLeadingDigit)
{
    EXPECT_TRUE(ConfigurationKey::is_valid_name("nexus.port"));
    EXPECT_FALSE(ConfigurationKey::is_valid_name(""));
    EXPECT_FALSE(ConfigurationKey::is_valid_name("1abc"));
}

TEST(ConfigurationValueTest, ReportsTypeForEachSupportedAlternative)
{
    EXPECT_EQ(ConfigurationValue{true}.type(), ConfigValueType::Bool);
    EXPECT_EQ(ConfigurationValue{std::int64_t{7}}.type(), ConfigValueType::Int);
    EXPECT_EQ(ConfigurationValue{1.5}.type(), ConfigValueType::Float);
    EXPECT_EQ(ConfigurationValue{std::string{"x"}}.type(), ConfigValueType::String);
}

TEST(ConfigurationValueTest, TypeSafeAccessorSucceedsForMatchingType)
{
    const ConfigurationValue value{std::int64_t{42}};
    const auto result = value.as_int();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
}

TEST(ConfigurationValueTest, TypeSafeAccessorFailsForMismatchedType)
{
    const ConfigurationValue value{std::int64_t{42}};
    const auto result = value.as_string();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code(), ErrorCode::ConfigInvalid);
}

TEST(ConfigurationValueTest, EqualityComparesHeldValue)
{
    EXPECT_EQ(ConfigurationValue{std::int64_t{1}}, ConfigurationValue{std::int64_t{1}});
    EXPECT_NE(ConfigurationValue{std::int64_t{1}}, ConfigurationValue{std::int64_t{2}});
    EXPECT_NE(ConfigurationValue{std::int64_t{1}}, ConfigurationValue{1.0});
}

TEST(ConfigurationValueTest, ToStringFormatsPerHeldType)
{
    EXPECT_EQ(to_string(ConfigurationValue{true}), "true");
    EXPECT_EQ(to_string(ConfigurationValue{std::string{"hi"}}), "\"hi\"");
}

} // namespace nexus::core::test
