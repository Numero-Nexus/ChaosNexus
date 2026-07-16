// Unit tests for the nexus::core assertion macros (Phase 8D).
// NEXUS_ASSERT/_MSG abort the process on failure, so they are
// exercised via EXPECT_DEATH in a subprocess; they compile out
// entirely under NDEBUG, matching production release builds.
// NEXUS_VERIFY/_MSG throw InternalException and are always active,
// so they are tested directly.

#include "nexus/core/error/assert.hpp"
#include "nexus/core/error/exception.hpp"

#include <gtest/gtest.h>

namespace nexus::core::test {

TEST(NexusVerify, DoesNotThrowWhenConditionHolds)
{
    EXPECT_NO_THROW(NEXUS_VERIFY(1 + 1 == 2));
}

TEST(NexusVerify, ThrowsInternalExceptionWhenConditionFails)
{
    EXPECT_THROW(NEXUS_VERIFY(1 + 1 == 3), InternalException);
}

TEST(NexusVerify, ThrownErrorCarriesPreconditionViolatedCode)
{
    try {
        NEXUS_VERIFY_MSG(false, "custom verify message");
        FAIL() << "expected NEXUS_VERIFY_MSG to throw";
    } catch (const InternalException& ex) {
        EXPECT_EQ(ex.error().code(), ErrorCode::PreconditionViolated);
        EXPECT_EQ(ex.error().message(), "custom verify message");
    }
}

#if !defined(NDEBUG)
TEST(NexusAssertDeathTest, AbortsWhenConditionFails)
{
    EXPECT_DEATH({ NEXUS_ASSERT(1 + 1 == 3); }, "");
}

TEST(NexusAssertDeathTest, DoesNotAbortWhenConditionHolds)
{
    EXPECT_NO_THROW(NEXUS_ASSERT(1 + 1 == 2));
}
#endif

} // namespace nexus::core::test
