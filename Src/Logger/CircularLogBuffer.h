#pragma once

#include <vector>
#include <mutex>
#include <optional>

#include "LogMessage.h"

class CircularLogBuffer
{
public:
    explicit CircularLogBuffer(size_t capacity)
        : capacity(capacity), buffer(capacity)
    {
    }

    void Push(const LogMessage& message)
    {
        std::lock_guard<std::mutex> lock(mutex);

        buffer[writeIndex] = message;
        writeIndex = (writeIndex + 1) % capacity;

        if (size < capacity)
        {
            ++size;
        }
        else
        {
            readIndex = (readIndex + 1) % capacity;  // Overwrite oldest
        }
    }

    std::vector<LogMessage> GetSnapshot()
    {
        std::lock_guard<std::mutex> lock(mutex);

        std::vector<LogMessage> snapshot;
        snapshot.reserve(size);

        for (size_t i = 0; i < size; ++i)
        {
            size_t index = (readIndex + i) % capacity;
            snapshot.push_back(buffer[index]);
        }

        return snapshot;
    }

    size_t GetSize() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return size;
    }

private:
    size_t capacity;
    std::vector<LogMessage> buffer;

    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t size = 0;

    mutable std::mutex mutex;
};
