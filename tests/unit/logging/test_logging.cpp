// Unit tests for nexus::core logging (Phase 8C — Logging Framework).
// Covers LogLevel ordering/formatting, runtime filtering, sink
// fan-out, and the basic record formatter.

#include "nexus/core/logging/console_sink.hpp"
#include "nexus/core/logging/log_format.hpp"
#include "nexus/core/logging/log_level.hpp"
#include "nexus/core/logging/logger.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace nexus::core::test {

namespace {

// Minimal in-memory sink used to assert on dispatched records
// without touching stdout/stderr.
class RecordingSink final : public LogSink {
public:
    auto write(LogLevel level, std::string_view formatted) -> void override
    {
        std::scoped_lock lock{mutex_};
        records_.emplace_back(level, std::string{formatted});
    }

    [[nodiscard]] auto records() const -> std::vector<std::pair<LogLevel, std::string>>
    {
        std::scoped_lock lock{mutex_};
        return records_;
    }

private:
    mutable std::mutex mutex_;
    std::vector<std::pair<LogLevel, std::string>> records_;
};

} // namespace

TEST(LogLevel, ToStringIsStable)
{
    EXPECT_EQ(to_string(LogLevel::Trace), "TRACE");
    EXPECT_EQ(to_string(LogLevel::Info), "INFO");
    EXPECT_EQ(to_string(LogLevel::Critical), "CRITICAL");
    EXPECT_EQ(to_string(LogLevel::Off), "OFF");
}

TEST(LogLevel, OrderingIsSeverityIncreasing)
{
    EXPECT_LT(LogLevel::Trace, LogLevel::Debug);
    EXPECT_LT(LogLevel::Debug, LogLevel::Info);
    EXPECT_LT(LogLevel::Info, LogLevel::Warn);
    EXPECT_LT(LogLevel::Warn, LogLevel::Error);
    EXPECT_LT(LogLevel::Error, LogLevel::Critical);
    EXPECT_LT(LogLevel::Critical, LogLevel::Off);
}

TEST(LogFormat, ProducesLevelAndMessage)
{
    const auto location = std::source_location::current();
    const std::string formatted = format_log_record(LogLevel::Warn, "disk nearly full", location);

    EXPECT_NE(formatted.find("[WARN]"), std::string::npos);
    EXPECT_NE(formatted.find("disk nearly full"), std::string::npos);
}

TEST(Logger, DispatchesToAllRegisteredSinks)
{
    Logger logger{LogLevel::Info};
    auto sink_a = std::make_shared<RecordingSink>();
    auto sink_b = std::make_shared<RecordingSink>();
    logger.add_sink(sink_a);
    logger.add_sink(sink_b);

    logger.log(LogLevel::Info, "hello");

    EXPECT_EQ(sink_a->records().size(), 1U);
    EXPECT_EQ(sink_b->records().size(), 1U);
}

TEST(Logger, RuntimeFilterDropsRecordsBelowMinLevel)
{
    Logger logger{LogLevel::Warn};
    auto sink = std::make_shared<RecordingSink>();
    logger.add_sink(sink);

    logger.log(LogLevel::Info, "should be dropped");
    logger.log(LogLevel::Error, "should pass");

    const auto records = sink->records();
    ASSERT_EQ(records.size(), 1U);
    EXPECT_EQ(records.front().first, LogLevel::Error);
}

TEST(Logger, SetLevelChangesFilteringAtRuntime)
{
    Logger logger{LogLevel::Error};
    EXPECT_FALSE(logger.is_enabled(LogLevel::Info));

    logger.set_level(LogLevel::Trace);
    EXPECT_TRUE(logger.is_enabled(LogLevel::Info));
    EXPECT_EQ(logger.level(), LogLevel::Trace);
}

TEST(Logger, OffLevelDisablesAllRecords)
{
    Logger logger{LogLevel::Off};
    auto sink = std::make_shared<RecordingSink>();
    logger.add_sink(sink);

    logger.log(LogLevel::Critical, "even critical is dropped");

    EXPECT_TRUE(sink->records().empty());
}

TEST(ConsoleSink, WriteDoesNotThrow)
{
    ConsoleSink sink;
    EXPECT_NO_THROW(sink.write(LogLevel::Info, "[INFO] test message"));
    EXPECT_NO_THROW(sink.write(LogLevel::Error, "[ERROR] test message"));
}

} // namespace nexus::core::test
