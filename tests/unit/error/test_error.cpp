// Unit tests for nexus::core Error and format_error (Phase 8D).

#include "nexus/core/error/error.hpp"
#include "nexus/core/error/error_format.hpp"

#include <gtest/gtest.h>
#include <string>

namespace nexus::core::test {

TEST(Error, StoresCodeMessageAndDerivedCategory)
{
    const Error error{ErrorCode::FileNotFound, "config.toml missing"};

    EXPECT_EQ(error.code(), ErrorCode::FileNotFound);
    EXPECT_EQ(error.category(), ErrorCategory::Io);
    EXPECT_EQ(error.message(), "config.toml missing");
}

TEST(Error, CapturesCallerSourceLocationByDefault)
{
    const auto expected_line = __LINE__ + 1;
    const Error error{ErrorCode::InternalError, "unreachable"};

    EXPECT_EQ(error.location().line(), expected_line);
}

TEST(Error, AcceptsExplicitSourceLocation)
{
    const auto location = std::source_location::current();
    const Error error{ErrorCode::Unknown, "explicit location", location};

    EXPECT_EQ(error.location().line(), location.line());
}

TEST(FormatError, ProducesCategoryCodeAndMessage)
{
    const Error error{ErrorCode::Timeout, "operation exceeded deadline"};
    const std::string formatted = format_error(error);

    EXPECT_NE(formatted.find("Runtime"), std::string::npos);
    EXPECT_NE(formatted.find("Timeout"), std::string::npos);
    EXPECT_NE(formatted.find("operation exceeded deadline"), std::string::npos);
}

} // namespace nexus::core::test
