// Unit tests for the nexus::core exception hierarchy (Phase 8D).

#include "nexus/core/error/error.hpp"
#include "nexus/core/error/exception.hpp"

#include <gtest/gtest.h>
#include <string_view>

namespace nexus::core::test {

TEST(NexusException, ExposesUnderlyingError)
{
    const NexusException ex{Error{ErrorCode::InternalError, "unexpected state"}};

    EXPECT_EQ(ex.error().code(), ErrorCode::InternalError);
    EXPECT_EQ(ex.error().message(), "unexpected state");
}

TEST(NexusException, WhatContainsCategoryCodeAndMessage)
{
    const NexusException ex{Error{ErrorCode::Timeout, "deadline exceeded"}};
    const std::string_view what{ex.what()};

    EXPECT_NE(what.find("Runtime"), std::string_view::npos);
    EXPECT_NE(what.find("Timeout"), std::string_view::npos);
    EXPECT_NE(what.find("deadline exceeded"), std::string_view::npos);
}

TEST(RuntimeException, IsCatchableAsNexusExceptionAndStdException)
{
    bool caught = false;
    try
    {
        throw RuntimeException{Error{ErrorCode::OperationFailed, "op failed"}};
    }
    catch (const NexusException& ex)
    {
        caught = true;
        EXPECT_EQ(ex.error().code(), ErrorCode::OperationFailed);
    }
    EXPECT_TRUE(caught) << "expected RuntimeException to be caught as NexusException";
}

TEST(ConfigurationException, IsCatchableAsStdException)
{
    bool caught = false;
    try
    {
        throw ConfigurationException{Error{ErrorCode::ConfigMissing, "missing key"}};
    }
    catch (const std::exception& ex)
    {
        caught = true;
        EXPECT_NE(std::string_view{ex.what()}.find("ConfigMissing"), std::string_view::npos);
    }
    EXPECT_TRUE(caught) << "expected ConfigurationException to be caught as std::exception";
}

TEST(InternalException, CarriesInternalCategory)
{
    const InternalException ex{Error{ErrorCode::PreconditionViolated, "invariant broken"}};
    EXPECT_EQ(ex.error().category(), ErrorCategory::Internal);
}

} // namespace nexus::core::test
