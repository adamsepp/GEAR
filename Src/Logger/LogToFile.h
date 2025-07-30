#pragma once

#include <fstream>
#include <mutex>
#include <filesystem>
#include <string>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <system_error>

class LogToFile
{
public:
    LogToFile(const std::string& folderPath,
        const std::string& fileName,
        size_t maxFileSizeKB = 10240,  // 10 MB default
        int maxBackups = 5)
        : folder(folderPath),
        filename(fileName),
        maxFileSize(maxFileSizeKB * 1024),
        maxBackups(maxBackups),
        stopFlag(false)
    {
        // Create directory for logs if not exists
        std::filesystem::create_directories(folder);

        // Open initial log file
        OpenLogFile();

        // Start background thread to process log queue asynchronously
        workerThread = std::thread(&LogToFile::ProcessQueue, this);
    }

    ~LogToFile()
    {
        // Signal background thread to stop and notify
        {
            std::lock_guard lock(queueMutex);
            stopFlag = true;
        }
        cv.notify_all();

        // Wait for background thread to finish processing remaining logs
        if (workerThread.joinable())
            workerThread.join();

        // Close the log file safely
        std::lock_guard lock(fileMutex);
        if (logStream.is_open())
            logStream.close();
    }

    // Thread-safe enqueue of log lines; wakes background thread
    void Write(const std::string& logLine)
    {
        {
            std::lock_guard lock(queueMutex);
            logQueue.push(logLine);
        }
        cv.notify_one();
    }

private:
    std::string folder;
    std::string filename;
    size_t maxFileSize;
    int maxBackups;

    std::ofstream logStream;
    std::mutex fileMutex;   // Protects file operations (open/write/rotate)

    std::queue<std::string> logQueue;
    std::mutex queueMutex;  // Protects the queue of pending log lines
    std::condition_variable cv;

    std::thread workerThread;
    std::atomic<bool> stopFlag;

    std::filesystem::path CurrentLogPath() const
    {
        return std::filesystem::path(folder) / filename;
    }

    void OpenLogFile()
    {
        std::lock_guard lock(fileMutex);
        logStream.open(CurrentLogPath(), std::ios::app);
    }

    // Helper: attempts to rename a file multiple times, retrying on failure
    bool TryRenameWithRetry(const std::filesystem::path& oldPath, const std::filesystem::path& newPath, int maxRetries = 5)
    {
        for (int attempt = 0; attempt < maxRetries; ++attempt)
        {
            std::error_code ec;
            std::filesystem::rename(oldPath, newPath, ec);
            if (!ec)
                return true;

            // Small delay before retrying to avoid race conditions
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return false;
    }

    // Rotation logic - rotates log files by renaming backups and creating a new log file
    // **Important:** This function MUST be called while holding 'fileMutex' lock externally,
    // so it does NOT lock 'fileMutex' internally to avoid deadlocks.
    void RotateFiles()
    {
        logStream.close();

        auto folderPath = std::filesystem::path(folder);

        std::error_code ec;

        // Remove oldest backup if it exists to keep maxBackups limit
        auto oldestBackup = folderPath / (filename + "." + std::to_string(maxBackups));
        if (std::filesystem::exists(oldestBackup, ec))
            std::filesystem::remove(oldestBackup, ec);

        // Shift backups: file.log.N -> file.log.N+1
        for (int i = maxBackups - 1; i >= 1; --i)
        {
            auto src = folderPath / (filename + "." + std::to_string(i));
            if (std::filesystem::exists(src, ec))
            {
                auto dst = folderPath / (filename + "." + std::to_string(i + 1));
                if (!TryRenameWithRetry(src, dst))
                {
                    // Optionally log or handle rename failure here
                }
            }
        }

        // Rename current log file to file.log.1 (first backup)
        auto currentLog = CurrentLogPath();
        auto backupOne = folderPath / (filename + ".1");
        if (!TryRenameWithRetry(currentLog, backupOne))
        {
            // Optionally handle rename failure here
        }

        // Reopen new log file for continued logging
        logStream.open(CurrentLogPath(), std::ios::app);
    }

    // Background thread method processing queued log lines asynchronously
    void ProcessQueue()
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        while (!stopFlag || !logQueue.empty())
        {
            // Wait for either new data or stop signal
            cv.wait(lock, [this]() { return stopFlag || !logQueue.empty(); });

            // Process all queued log lines
            while (!logQueue.empty())
            {
                // Pop next log line from queue
                std::string line = std::move(logQueue.front());
                logQueue.pop();

                // Unlock queue mutex while writing to avoid blocking producers
                lock.unlock();

                {
                    // Lock file mutex to protect file access and possible rotation
                    std::lock_guard fileLock(fileMutex);

                    if (!logStream.is_open())
                        OpenLogFile();

                    // Write log line and flush to ensure persistence
                    logStream << line << std::endl;
                    logStream.flush();

                    // Check file size and rotate if necessary
                    std::error_code ec;
                    auto fileSize = std::filesystem::file_size(CurrentLogPath(), ec);
                    if (!ec && fileSize > maxFileSize)
                        RotateFiles(); // Called without locking fileMutex inside to avoid deadlock
                }

                // Re-lock queue mutex before next iteration
                lock.lock();
            }
        }
    }
};
