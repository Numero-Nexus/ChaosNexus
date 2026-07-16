// Unit tests for nexus::core Result<T> and its helper utilities
// (Phase 8D). Covers value/error storage, move semantics, and the
// success()/failure()/has_value()/value_or()/error() helpers.

#include "nexus/core/error/error.hpp"
#include "nexus/core/error/helpers.hpp"
#include "nexus/core/error/result.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

namespace nexus::core::test {

TEST(Result, HoldsValueOnSuccessConstruction)
{
    Result<int> result{42};

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(static_cast<bool>(result));
    EXPECT_EQ(result.value(), 42);
}

TEST(Result, HoldsErrorOnFailureConstruction)
{
    Result<int> result{Error{ErrorCode::NotFound, "missing entry"}};

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(static_cast<bool>(result));
    EXPECT_EQ(result.error().code(), ErrorCode::NotFound);
}

TEST(Result, ValueOrReturnsValueWhenPresent)
{
    const Result<int> result{7};
    EXPECT_EQ(result.value_or(-1), 7);
}

TEST(Result, ValueOrReturnsFallbackOnError)
{
    const Result<int> result{Error{ErrorCode::Unknown, "n/a"}};
    EXPECT_EQ(result.value_or(-1), -1);
}

TEST(Result, MoveOnlyValueTypeIsSupported)
{
    Result<std::unique_ptr<int>> result{std::make_unique<int>(99)};

    ASSERT_TRUE(result.has_value());
    auto owned = std::move(result).value();
    EXPECT_EQ(*owned, 99);
}

TEST(Result, RvalueErrorAccessorMoves)
{
    Result<int> result{Error{ErrorCode::IoFailure, "disk error"}};
    Error moved_error = std::move(result).error();

    EXPECT_EQ(moved_error.code(), ErrorCode::IoFailure);
}

TEST(ResultHelpers, SuccessProducesValueHoldingResult)
{
    const auto result = success<int>(5);

    EXPECT_TRUE(has_value(result));
    EXPECT_EQ(result.value(), 5);
}

TEST(ResultHelpers, FailureWithCodeAndMessageProducesErrorHoldingResult)
{
    const auto result = failure<std::string>(ErrorCode::ConfigInvalid, "bad value");

    EXPECT_FALSE(has_value(result));
    EXPECT_EQ(error(result).code(), ErrorCode::ConfigInvalid);
    EXPECT_EQ(error(result).message(), "bad value");
}

TEST(ResultHelpers, FailureWithExistingErrorIsForwarded)
{
    Error original{ErrorCode::PermissionDenied, "no access"};
    const auto result = failure<int>(original);

    EXPECT_FALSE(has_value(result));
    EXPECT_EQ(error(result).code(), ErrorCode::PermissionDenied);
}

TEST(ResultHelpers, ValueOrFreeFunctionMatchesMember)
{
    const auto ok = success<int>(3);
    const auto err = failure<int>(ErrorCode::Unknown, "n/a");

    EXPECT_EQ(value_or(ok, -1), 3);
    EXPECT_EQ(value_or(err, -1), -1);
}

} // namespace nexus::core::test
