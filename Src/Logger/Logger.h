#pragma once

#include <string>
#include <vector>
#include <fmt/core.h>
#include <fmt/format.h>

#include "LogMessage.h"
#include "CircularLogBuffer.h"

class Logger
{
public:
    template<typename... Args>
    static void Log(LogLevel level, fmt::format_string<Args...> formatStr, Args&&... args)
    {
        try
        {
            std::string formatted = fmt::format(formatStr, std::forward<Args>(args)...);
            PushToBuffer(level, formatted);
        }
        catch (const fmt::format_error& e)
        {
            std::string fallback = "[LOG FORMAT ERROR] ";
            fallback += e.what();
            fallback += " | Format: ";
            fallback += formatStr.get().data();
            PushToBuffer(LogLevel::Error, fallback);
        }
    }

    static std::vector<LogMessage> GetSnapshot();
    static size_t GetLogCount();

private:
    static void PushToBuffer(LogLevel level, const std::string& message);
    static void Write(const LogMessage& message);

    static inline CircularLogBuffer logBuffer{ 1000 };
};

// Logging macros
#define LOG_INFO(...)  Logger::Log(LogLevel::Info, __VA_ARGS__)
#define LOG_WARN(...)  Logger::Log(LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Log(LogLevel::Error, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::Log(LogLevel::Debug, __VA_ARGS__)
