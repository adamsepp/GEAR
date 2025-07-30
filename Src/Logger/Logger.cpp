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
    fileLogger.Write(message);
}
