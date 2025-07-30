#pragma once

#include <vector>
#include <mutex>

#include "LogMessage.h"

class CircularLogBuffer
{
public:
    explicit CircularLogBuffer(size_t capacity)
        : capacity(capacity), buffer(capacity)
    {
    }

    // Thread-safe push: writes are synchronized via mutex.
    // Supports concurrent pushes from multiple threads.
    void Push(const LogMessage& message)
    {
        std::lock_guard<std::mutex> lock(mutex);

        buffer[writeIndex] = message;
        writeIndex = (writeIndex + 1) % capacity;

        if (size < capacity)
            ++size;
        else
            readIndex = (readIndex + 1) % capacity; // Overwrite oldest
    }

    // -------- Read Accessors --------

    // Returns const reference to the internal ring buffer.
    // Thread-safe if consumer (e.g. GUI) only reads from this vector.
    // Caller must use GetReadIndex() and GetSize() to iterate in correct order.
    const std::vector<LogMessage>& GetBuffer() const { return buffer; }

    // Thread-safe: readIndex is only written inside Push() under lock.
    // Read-only here. On modern 64-bit systems, aligned size_t reads are atomic.
    size_t GetReadIndex() const { return readIndex; }

    // Thread-safe: size is only written inside Push() under lock.
    // Read-only access here is safe and avoids unnecessary locking in GUI thread.
    size_t GetSize() const { return size; }

private:
    size_t capacity;
    std::vector<LogMessage> buffer;

    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t size = 0;

    mutable std::mutex mutex; // protects Push() against concurrent writes
};
