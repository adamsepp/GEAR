#pragma once

#include <string>
#include <vector>
#include <fmt/core.h>
#include <fmt/format.h>

#include "LogMessage.h"
#include "LogToFile.h"
#include "CircularLogBuffer.h"

class Logger
{
public:
	static constexpr size_t LOG_BUFFER_CAPACITY = 10000;

	// Just user message --> 'MyFunction(): Some message'
	template<typename... Args>
	static void Log(const char* callerFunc, LogLevel level, fmt::format_string<Args...> formatStr, Args&&... args)
	{
		std::string formatted = fmt::format("{}(): {}", callerFunc, fmt::format(formatStr, std::forward<Args>(args)...));
		PushToBuffer(level, std::move(formatted));
	}
	// Just user message with no args
	static void Log(const char* callerFunc, LogLevel level, std::string_view message)
	{
		std::string formatted = fmt::format("{}(): {}", callerFunc, message);
		PushToBuffer(level, std::move(formatted));
	}

	// Object as prefix --> 'ObjectXY MyFunction(): Some message'
	template<typename... Args>
	static void Log1(const char* callerFunc, LogLevel level, const std::string& object, fmt::format_string<Args...> formatStr, Args&&... args)
	{
		std::string formatted = fmt::format("{} {}(): {}", object, callerFunc, fmt::format(formatStr, std::forward<Args>(args)...));
		PushToBuffer(level, std::move(formatted));
	}
	// Object as prefix with no args
	static void Log1(const char* callerFunc, LogLevel level, const std::string& object, std::string_view message)
	{
		std::string formatted = fmt::format("{} {}(): {}", object, callerFunc, message);
		PushToBuffer(level, std::move(formatted));
	}

	// Object and name as prefix --> 'ObjectXY "Stone" MyFunction(): Some message'
	template<typename... Args>
	static void Log2(const char* callerFunc, LogLevel level, const std::string& object, const std::string& name, fmt::format_string<Args...> formatStr, Args&&... args)
	{
		std::string formatted = fmt::format("{} \"{}\" {}(): {}", object, name, callerFunc, fmt::format(formatStr, std::forward<Args>(args)...));
		PushToBuffer(level, std::move(formatted));
	}
	// Object and name as prefix with no args
	static void Log2(const char* callerFunc, LogLevel level, const std::string& object, const std::string& name, std::string_view message)
	{
		std::string formatted = fmt::format("{} \"{}\" {}(): {}", object, name, callerFunc, message);
		PushToBuffer(level, std::move(formatted));
	}

	// Caller, object and name as prefix --> 'CallerXY >> ObjectXY "Stone" MyFunction(): Some message'
	template<typename... Args>
	static void Log3(const char* callerFunc, LogLevel level, const std::string& caller, const std::string& object, const std::string& name, fmt::format_string<Args...> formatStr, Args&&... args)
	{
		std::string formatted = fmt::format("{} >> {} \"{}\" {}(): {}", caller, object, name, callerFunc, fmt::format(formatStr, std::forward<Args>(args)...));
		PushToBuffer(level, std::move(formatted));
	}
	// Caller, object and name as prefix with no args
	static void Log3(const char* callerFunc, LogLevel level, const std::string& caller, const std::string& object, const std::string& name, std::string_view message)
	{
		std::string formatted = fmt::format("{} >> {} \"{}\" {}(): {}", caller, object, name, callerFunc, message);
		PushToBuffer(level, std::move(formatted));
	}

	static const std::vector<LogMessage>& GetBuffer() { return logBuffer.GetBuffer(); }
	static size_t GetReadIndex() { return logBuffer.GetReadIndex(); }
	static size_t GetSize() { return logBuffer.GetSize(); }

	static bool ShouldScrollToBottom() { return scrollToBottom.exchange(false); } // resets after check

private:
	static void PushToBuffer(LogLevel level, const std::string& message);
	static void Write(const LogMessage& message);

	static inline CircularLogBuffer logBuffer{ LOG_BUFFER_CAPACITY };
	static inline std::atomic_bool scrollToBottom{ false };

	static inline LogToFile fileLogger{ ".\\Log", "CppSandbox.log", 10240, 5 };
};

// Logging macros

// Level 0 - Just message
#define LOG_INFO(...)  Logger::Log(__func__, LogLevel::Info, __VA_ARGS__)
#define LOG_WARN(...)  Logger::Log(__func__, LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Log(__func__, LogLevel::Error, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::Log(__func__, LogLevel::Debug, __VA_ARGS__)

// Level 1 - Object + message
#define LOG1_INFO(obj, ...)  Logger::Log1(__func__, LogLevel::Info, obj, __VA_ARGS__)
#define LOG1_WARN(obj, ...)  Logger::Log1(__func__, LogLevel::Warning, obj, __VA_ARGS__)
#define LOG1_ERROR(obj, ...) Logger::Log1(__func__, LogLevel::Error, obj, __VA_ARGS__)
#define LOG1_DEBUG(obj, ...) Logger::Log1(__func__, LogLevel::Debug, obj, __VA_ARGS__)

// Level 2 - Object + Name + message
#define LOG2_INFO(obj, name, ...)  Logger::Log2(__func__, LogLevel::Info, obj, name, __VA_ARGS__)
#define LOG2_WARN(obj, name, ...)  Logger::Log2(__func__, LogLevel::Warning, obj, name, __VA_ARGS__)
#define LOG2_ERROR(obj, name, ...) Logger::Log2(__func__, LogLevel::Error, obj, name, __VA_ARGS__)
#define LOG2_DEBUG(obj, name, ...) Logger::Log2(__func__, LogLevel::Debug, obj, name, __VA_ARGS__)

// Level 3 - Caller + Object + Name + message
#define LOG3_INFO(caller, obj, name, ...)  Logger::Log3(__func__, LogLevel::Info, caller, obj, name, __VA_ARGS__)
#define LOG3_WARN(caller, obj, name, ...)  Logger::Log3(__func__, LogLevel::Warning, caller, obj, name, __VA_ARGS__)
#define LOG3_ERROR(caller, obj, name, ...) Logger::Log3(__func__, LogLevel::Error, caller, obj, name, __VA_ARGS__)
#define LOG3_DEBUG(caller, obj, name, ...) Logger::Log3(__func__, LogLevel::Debug, caller, obj, name, __VA_ARGS__)
