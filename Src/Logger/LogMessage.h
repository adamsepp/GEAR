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
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::thread::id threadId;
    std::string source; // optional: filename, tag etc.

    LogMessage()
        : timestamp(std::chrono::system_clock::now()),
        level(LogLevel::Info),
        message(""),
        threadId(std::this_thread::get_id()),
        source("")
    {
    }
    LogMessage(LogLevel level, std::string message, std::string source = "")
        : timestamp(std::chrono::system_clock::now()),
        level(level),
        message(std::move(message)),
        threadId(std::this_thread::get_id()),
        source(std::move(source))
    {
    }
};
