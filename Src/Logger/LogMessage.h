#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <fmt/core.h>

enum class LogLevel
{
	Info,
	Warning,
	Error,
	Debug
};

struct LogMessageColor
{
	float r, g, b, a;
	constexpr LogMessageColor(float r, float g, float b, float a = 1.0f)
		: r(r), g(g), b(b), a(a) {}
};

struct LogMessage
{
	LogLevel level;
	std::chrono::system_clock::time_point timestamp;
	std::string message;

	LogMessage()
		: level(LogLevel::Info),
		timestamp(std::chrono::system_clock::now()),
		message("")
	{}

	LogMessage(LogLevel level, std::string msg)
		: level(level),
		message(std::move(msg)),
		timestamp(std::chrono::system_clock::now())
	{}

	const char* FormatLevel() const
	{
		switch (level)
		{
		case LogLevel::Info:    return "INFO";
		case LogLevel::Warning: return "WARN";
		case LogLevel::Error:   return "ERROR";
		case LogLevel::Debug:   return "DEBUG";
		default:                return "UNKNOWN";
		}
	}

	LogMessageColor LevelColor() const
	{
		switch (level)
		{
		case LogLevel::Info:    return { 1, 1, 1, 1 };
		case LogLevel::Warning: return { 1, 1, 0, 1 };
		case LogLevel::Error:   return { 1, 0.3f, 0.3f, 1 };
		case LogLevel::Debug:   return { 0.5f, 0.5f, 1, 1 };
		default:                return { 0.5, 0.5, 0.5, 0.5 };
		}
	}

	void FormatTimestamp(char* outBuffer, size_t bufferSize) const
	{
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()) % 1000;
		std::time_t t = std::chrono::system_clock::to_time_t(timestamp);
		std::tm tm;

#ifdef _WIN32
		localtime_s(&tm, &t);
#else
		localtime_r(&t, &tm);
#endif

		char tempBuffer[64];
		std::strftime(tempBuffer, sizeof(tempBuffer), "[%Y:%m:%d %H:%M:%S", &tm);
		std::snprintf(outBuffer, bufferSize, "%s.%03lld]", tempBuffer, static_cast<long long>(ms.count()));
	}

	std::string ToStringForFile() const
	{
		char timeString[80];
		FormatTimestamp(timeString, sizeof(timeString));
		return fmt::format("[{0}] {1} {2}", FormatLevel(), timeString, message);
	}
};
