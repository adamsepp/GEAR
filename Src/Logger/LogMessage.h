#pragma once

#include <string>
#include <chrono>
#include <thread>

enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

struct LogMessage
{
    LogLevel level;
    std::chrono::system_clock::time_point timestamp;
    std::string timeFormatted;
    std::string message;
    std::thread::id threadId;

    // Default constructor
    LogMessage()
        : level(LogLevel::Info),
        timestamp(std::chrono::system_clock::now()),
        message(""),
        threadId(std::this_thread::get_id()),
        timeFormatted("")
    {
    }

    // Custom constructor
    LogMessage(LogLevel level, std::string message)
        : level(level),
        timestamp(std::chrono::system_clock::now()),
        message(std::move(message)),
        threadId(std::this_thread::get_id()),
        timeFormatted(FormatTimestamp(timestamp))
    {
    }

private:
    static std::string FormatTimestamp(const std::chrono::system_clock::time_point& tp)
    {
        using namespace std::chrono;

        auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;
        std::time_t t = system_clock::to_time_t(tp);
        std::tm tm;

#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif

        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "[%Y:%m:%d %H:%M:%S", &tm);
        char result[80];
        std::snprintf(result, sizeof(result), "%s.%03lld]", buffer, static_cast<long long>(ms.count()));
        return result;
    }
};
