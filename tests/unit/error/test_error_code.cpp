// Unit tests for nexus::core ErrorCode / ErrorCategory (Phase 8D —
// Error Handling Framework). Covers string conversion and the
// ErrorCode -> ErrorCategory mapping.

#include "nexus/core/error/error_category.hpp"
#include "nexus/core/error/error_code.hpp"

#include <gtest/gtest.h>

namespace nexus::core::test {

TEST(ErrorCode, ToStringIsStable)
{
    EXPECT_EQ(to_string(ErrorCode::None), "None");
    EXPECT_EQ(to_string(ErrorCode::InvalidArgument), "InvalidArgument");
    EXPECT_EQ(to_string(ErrorCode::IoFailure), "IoFailure");
    EXPECT_EQ(to_string(ErrorCode::InternalError), "InternalError");
}

TEST(ErrorCategory, ToStringIsStable)
{
    EXPECT_EQ(to_string(ErrorCategory::General), "General");
    EXPECT_EQ(to_string(ErrorCategory::Configuration), "Configuration");
    EXPECT_EQ(to_string(ErrorCategory::Io), "Io");
    EXPECT_EQ(to_string(ErrorCategory::Runtime), "Runtime");
    EXPECT_EQ(to_string(ErrorCategory::Internal), "Internal");
}

TEST(ErrorCategory, MapsEachErrorCodeToExpectedCategory)
{
    EXPECT_EQ(category_of(ErrorCode::InvalidArgument), ErrorCategory::General);
    EXPECT_EQ(category_of(ErrorCode::ConfigMissing), ErrorCategory::Configuration);
    EXPECT_EQ(category_of(ErrorCode::FileNotFound), ErrorCategory::Io);
    EXPECT_EQ(category_of(ErrorCode::Timeout), ErrorCategory::Runtime);
    EXPECT_EQ(category_of(ErrorCode::NotImplemented), ErrorCategory::Internal);
}

TEST(ErrorCategory, MappingIsConstexprEvaluable)
{
    static_assert(category_of(ErrorCode::PermissionDenied) == ErrorCategory::Io);
    static_assert(category_of(ErrorCode::PreconditionViolated) == ErrorCategory::Internal);
}

} // namespace nexus::core::test
