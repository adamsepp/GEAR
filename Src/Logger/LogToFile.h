#pragma once

#include <fstream>
#include <mutex>
#include <filesystem>
#include <string>

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
        maxBackups(maxBackups)
    {
        std::filesystem::create_directories(folder);
        OpenLogFile();
    }

    void Write(const std::string& logLine)
    {
        std::lock_guard lock(mutex);

        if (!logStream.is_open())
            OpenLogFile();

        logStream << logLine << std::endl;
        logStream.flush();

        if (std::filesystem::file_size(CurrentLogPath()) > maxFileSize)
        {
            RotateFiles();
            OpenLogFile();
        }
    }

private:
    std::string folder;
    std::string filename;
    size_t maxFileSize;
    int maxBackups;
    std::ofstream logStream;
    std::mutex mutex;

    std::filesystem::path CurrentLogPath() const
    {
        return std::filesystem::path(folder) / filename;
    }

    void OpenLogFile()
    {
        logStream.open(CurrentLogPath(), std::ios::app);
    }

    void RotateFiles()
    {
        logStream.close();

        auto folderPath = std::filesystem::path(folder);

        // Delete oldest backup if it exists
        auto oldestBackup = folderPath / (filename + "." + std::to_string(maxBackups));
        if (std::filesystem::exists(oldestBackup))
            std::filesystem::remove(oldestBackup);

        // Shift backups: file.log.(N-1) -> file.log.N
        for (int i = maxBackups - 1; i >= 1; --i)
        {
            auto src = folderPath / (filename + "." + std::to_string(i));
            if (std::filesystem::exists(src))
            {
                auto dst = folderPath / (filename + "." + std::to_string(i + 1));
                std::filesystem::rename(src, dst);
            }
        }

        // Rename current log file to file.log.1
        std::filesystem::rename(CurrentLogPath(), folderPath / (filename + ".1"));
    }
};
