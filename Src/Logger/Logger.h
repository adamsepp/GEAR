#pragma once

#include <string>
#include <vector>
#include <format>

#include "LogMessage.h"
#include "CircularLogBuffer.h"

class Logger
{
public:
    template<typename... Args>
    static void Log(LogLevel level, const std::string& formatStr, Args&&... args)
    {
        try
        {
            std::string formatted = std::vformat(formatStr, std::make_format_args(args...));
            PushToBuffer(level, formatted);
        }
        catch (const std::format_error& e)
        {
            std::string fallback = "[LOG FORMAT ERROR] " + std::string(e.what()) + " | Format: " + formatStr;
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
