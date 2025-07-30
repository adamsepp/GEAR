#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>

#include "Logger/Logger.h"
#include "Logger/LogToFile.h"

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
    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([i]()
            {
                for (int j = 0; j < logsPerThread; ++j)
                    Logger::Log(LogLevel::Info, "Thread {} - log {}", i, j);
            });
    }

    for (auto& t : threads)
        t.join();

    EXPECT_GE(Logger::GetSize(), 1);
}

// Performance: Log 10,000 entries in under 100 ms (single-threaded)
TEST(LoggerTest, LoggingPerformance_SingleThreaded)
{
    constexpr int logCount = 10000;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < logCount; ++i)
        Logger::Log(LogLevel::Debug, "Perf {}", i);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_LT(elapsedMs, 1000); // Fail if logging is too slow
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
        threads.emplace_back([=]
            {
                for (int i = 0; i < logsPerThread; ++i)
                    Logger::Log(LogLevel::Debug, "Thread {} - {}", t, i);
            });
    }

    for (auto& thread : threads)
        thread.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_LT(elapsedMs, 1000); // Ensure it finishes quickly under concurrent load
}

// ------------------------------
// File Logging Tests
// ------------------------------

// File Logging: Write a single line and verify content in the log file
TEST(LoggerFileTest, SimpleWrite)
{
    // Clean up any existing test logs
    std::filesystem::remove_all("test_logs");

    {
        // Create logger instance and write a test line
        LogToFile logger("test_logs", "test.log", 1024, 2);
        logger.Write("Test line 1");
    } // Logger destroyed here, file handles are closed

    auto path = std::filesystem::path("test_logs") / "test.log";
    ASSERT_TRUE(std::filesystem::exists(path));

    // Read back the line and verify content
    std::ifstream file(path);
    std::string line;
    std::getline(file, line);
    file.close();

    ASSERT_EQ(line, "Test line 1");

    // Clean up test logs after test
    std::filesystem::remove_all("test_logs");
}

// File Logging: Write enough data to trigger rotation and check backups
TEST(LoggerFileTest, Rotation)
{
    std::filesystem::remove_all("test_logs");

    const size_t maxSizeKB = 1; // small size to trigger rotation quickly
    const int maxBackups = 3;

    {
        LogToFile logger("test_logs", "test.log", maxSizeKB, maxBackups);

        // Write many large lines to exceed max file size and trigger rotation
        std::string bigLine(1024, 'x');
        for (int i = 0; i < 2000; ++i)
            logger.Write(bigLine);
    } // Logger destroyed here, file handles are closed

    auto folderPath = std::filesystem::path("test_logs");

    // Verify that all expected backup files exist
    for (int i = 1; i <= maxBackups; ++i)
    {
        auto backupPath = folderPath / ("test.log." + std::to_string(i));
        ASSERT_TRUE(std::filesystem::exists(backupPath)) << "Backup file " << backupPath << " missing";
    }

    // Verify no backups beyond the maxBackups count exist
    auto extraBackupPath = folderPath / ("test.log." + std::to_string(maxBackups + 1));
    ASSERT_FALSE(std::filesystem::exists(extraBackupPath)) << "Unexpected extra backup file found: " << extraBackupPath;

    std::filesystem::remove_all("test_logs");
}

// File Logging: Test thread safety by logging concurrently from multiple threads
TEST(LoggerFileTest, ThreadSafety)
{
    std::filesystem::remove_all("test_logs");

    {
        LogToFile logger("test_logs", "test.log", 10240, 5);

        // Function that writes many log lines to test concurrency
        auto threadFunc = [&logger]()
            {
                for (int i = 0; i < 500; ++i)
                    logger.Write("Thread-safe log line");
            };

        // Start multiple threads writing simultaneously
        std::thread t1(threadFunc);
        std::thread t2(threadFunc);
        std::thread t3(threadFunc);

        // Wait for all threads to finish
        t1.join();
        t2.join();
        t3.join();
    } // Logger destroyed here, file handles are closed

    auto path = std::filesystem::path("test_logs") / "test.log";
    ASSERT_TRUE(std::filesystem::exists(path));
    ASSERT_GT(std::filesystem::file_size(path), 0);

    std::filesystem::remove_all("test_logs");
}
