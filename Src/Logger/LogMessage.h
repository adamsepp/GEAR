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
    struct LogMessageColor
    {
        float r, g, b, a;
        constexpr LogMessageColor(float r, float g, float b, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}
    };

    LogLevel level;
    std::string levelStr;
    LogMessageColor levelColor;

    std::chrono::system_clock::time_point timestamp;
    std::string timeFormatted;
    std::string message;
    std::thread::id threadId;

    LogMessage()
        : level(LogLevel::Info),
        levelStr("INFO"),
        levelColor(1, 1, 1, 1),
        timestamp(std::chrono::system_clock::now()),
        timeFormatted(""),
        message(""),
        threadId(std::this_thread::get_id())
    {}

    LogMessage(LogLevel level, std::string msg)
        : level(level),
        message(std::move(msg)),
        levelColor(1, 1, 1, 1),
        timestamp(std::chrono::system_clock::now()),
        threadId(std::this_thread::get_id()),
        timeFormatted(FormatTimestamp(timestamp))
    {
        SetLevelMeta(level);
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

    void SetLevelMeta(LogLevel lvl)
    {
        switch (lvl)
        {
        case LogLevel::Info:
            levelStr = "INFO";
            levelColor = { 1, 1, 1, 1 };
            break;
        case LogLevel::Warning:
            levelStr = "WARN";
            levelColor = { 1, 1, 0, 1 };
            break;
        case LogLevel::Error:
            levelStr = "ERROR";
            levelColor = { 1, 0.3f, 0.3f, 1 };
            break;
        case LogLevel::Debug:
            levelStr = "DEBUG";
            levelColor = { 0.5f, 0.5f, 1, 1 };
            break;
        }
    }
};
