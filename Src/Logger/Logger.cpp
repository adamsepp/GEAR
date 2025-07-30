#include "Logger.h"

void Logger::PushToBuffer(LogLevel level, const std::string& message)
{
    LogMessage logMessage(level, message);
    logBuffer.Push(logMessage);

    scrollToBottom.store(true); // Mark that GUI should scroll to latest log after this push

    Write(logMessage);
}

void Logger::Write(const LogMessage& message)
{
    // Use the formatted time string from the message
    const std::string& timeStr = message.timeFormatted;

    // Format a line: "[LEVEL] [Time] Message"
    std::string logLine = fmt::format("[{0}] [{1}] {2}", message.levelStr, timeStr, message.message);

    // Write to the file (fileLogger is a static member in Logger)
    fileLogger.Write(logLine);
}
