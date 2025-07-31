#include "Logger.h"

void Logger::PushToBuffer(LogLevel level, const std::string& message)
{
	LogMessage logMessage(level, message);
	logBuffer.Push(logMessage);

	// Mark that GUI should scroll to latest log after this push
	scrollToBottom.store(true);

	// Write to file
	fileLogger.Write(message);
}
