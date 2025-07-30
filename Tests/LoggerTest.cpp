#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "Logger/Logger.h"

// Basic: Verify that messages are stored in correct order and with correct log levels
TEST(LoggerTest, StoresLogsInOrder)
{
    Logger::Log(LogLevel::Info, "First log");
    Logger::Log(LogLevel::Warning, "Second log");

    const auto& buffer = Logger::GetBuffer();
    const size_t size = Logger::GetSize();
    const size_t readIndex = Logger::GetReadIndex();
    const size_t capacity = buffer.size();

    ASSERT_GE(size, 2);

    const auto& msg1 = buffer[(readIndex + size - 2) % capacity];
    const auto& msg2 = buffer[(readIndex + size - 1) % capacity];

    EXPECT_EQ(msg1.level, LogLevel::Info);
    EXPECT_EQ(msg2.level, LogLevel::Warning);
    EXPECT_EQ(msg1.message, "First log");
    EXPECT_EQ(msg2.message, "Second log");
}

// Buffer Logic: Fill buffer beyond its capacity and check that oldest entries are overwritten
TEST(LoggerTest, RingBufferOverwritesOldEntries)
{
    const size_t capacity = Logger::LOG_BUFFER_CAPACITY;

    for (size_t i = 0; i < capacity + 5; ++i)
        Logger::Log(LogLevel::Debug, "Overflow test {}", i);

    EXPECT_EQ(Logger::GetSize(), capacity);

    const auto& buffer = Logger::GetBuffer();
    const size_t readIndex = Logger::GetReadIndex();
    const auto& firstMsg = buffer[readIndex];

    // The first visible log should be "Overflow test 5"
    EXPECT_TRUE(firstMsg.message.find("Overflow test 5") != std::string::npos);
}

// Thread Safety: Spawn multiple threads that write logs concurrently and verify no crash or data corruption
TEST(LoggerTest, ThreadSafety_MultipleThreadsWrite)
{
    constexpr int threadCount = 8;
    constexpr int logsPerThread = 100;

    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < logsPerThread; ++j) {
                Logger::Log(LogLevel::Info, "Thread {} - log {}", i, j);
            }
            });
    }

    for (auto& t : threads)
        t.join();

    // We don't expect all logs to remain due to limited buffer, but it should not crash or misbehave
    EXPECT_GE(Logger::GetSize(), 1);
}

// Performance: Log 10,000 entries in under 100 ms (single-threaded)
TEST(LoggerTest, LoggingPerformance_SingleThreaded)
{
    constexpr int logCount = 10000;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < logCount; ++i) {
        Logger::Log(LogLevel::Debug, "Perf {}", i);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_LT(elapsedMs, 100); // Fail if logging is too slow
}

// Performance + Thread-Safety: Log 10,000 entries from 4 threads in under 100 ms
TEST(LoggerTest, LoggingPerformance_MultiThreaded)
{
    constexpr int totalLogs = 10000;
    constexpr int numThreads = 4;
    constexpr int logsPerThread = totalLogs / numThreads;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t)
    {
        threads.emplace_back([=] {
            for (int i = 0; i < logsPerThread; ++i)
                Logger::Log(LogLevel::Debug, "Thread {} - {}", t, i);
            });
    }

    for (auto& thread : threads)
        thread.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_LT(elapsedMs, 100); // Ensure it finishes quickly under concurrent load
}
