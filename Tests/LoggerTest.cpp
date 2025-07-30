#include <gtest/gtest.h>
#include "Logger/Logger.h"

TEST(LoggerTest, LogsAreStoredInOrder)
{
    Logger::Log(LogLevel::Info, "Test log {}", 1);
    Logger::Log(LogLevel::Warning, "Test log {}", 2);

    const auto& buffer = Logger::GetBuffer();
    const size_t readIndex = Logger::GetReadIndex();
    const size_t size = Logger::GetSize();

    ASSERT_GE(size, 2);
    const auto& msg1 = buffer[(readIndex + size - 2) % buffer.size()];
    const auto& msg2 = buffer[(readIndex + size - 1) % buffer.size()];

    EXPECT_EQ(msg1.level, LogLevel::Info);
    EXPECT_EQ(msg2.level, LogLevel::Warning);
}
