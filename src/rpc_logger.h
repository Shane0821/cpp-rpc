#ifndef _RPC_LOGGER_H
#define _RPC_LOGGER_H

#include <fmt/core.h>

#include <atomic>
#include <ctime>
#include <filesystem>
#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "mpmc_queue.h"
#include "singleton.h"

class Logger : public Singleton<Logger> {
    friend class Singleton<Logger>;

   public:
    enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, FATAL };

    ~Logger();

    // Initialize logger with output file
    void init(LogLevel level = LogLevel::INFO, const std::string& filename = "");

    // add a log task to the queue
    template <typename... Args>
    constexpr void log(LogLevel level, int line, const std::string& format = "{}",
                       Args&&... args) {
        if (level < level_) return;

        taskQueue_.emplace(
            [... args = std::forward<Args>(args), level, line, format, this]() {
                time_t now = time(0);
                char* dt = ctime(&now);
                dt[strlen(dt) - 1] = '\0';  // Remove newline

                const char *levelStr = "";
                switch (level) {
                    case LogLevel::TRACE:
                        levelStr = "TRACE";
                        break;
                    case LogLevel::DEBUG:
                        levelStr = "DEBUG";
                        break;
                    case LogLevel::INFO:
                        levelStr = "INFO";
                        break;
                    case LogLevel::WARNING:
                        levelStr = "WARNING";
                        break;
                    case LogLevel::ERROR:
                        levelStr = "ERROR";
                        break;
                    case LogLevel::FATAL:
                        levelStr = "FATAL";
                        break;
                }

                auto logLine = fmt::format(
                    "[{}][{}][{}][{}:{}]:{}\n",
                    std::this_thread::get_id(),
                    dt,
                    levelStr,
                    __FILE__,
                    line,
                    fmt::format(format, std::forward<Args>(args)...)
                );

                fwrite(logLine.data(), sizeof(char), logLine.size(), logFile_);
            });
    }

    // flush the data from cache to the file
    void flush();

    std::string getLogFileName() const { return filename_; }
    Logger::LogLevel getLogLevel() const { return level_; }

   protected:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

   private:
    // Process log tasks
    void processLogTasks();

    // Tool function for getting default log file name
    std::string genDefaultLogFileName();

    FILE* logFile_{nullptr};
    std::string filename_;

    std::mutex mutex_;
    MPMCQueue<std::function<void()>> taskQueue_;
    std::thread processThread_;
    std::atomic<bool> stop_{false};

    LogLevel level_{LogLevel::INFO};
};

#define LOG_TRACE(format, ...) \
    Logger::GetInst().log(Logger::LogLevel::TRACE, __LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
    Logger::GetInst().log(Logger::LogLevel::DEBUG, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) \
    Logger::GetInst().log(Logger::LogLevel::INFO, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) \
    Logger::GetInst().log(Logger::LogLevel::WARNING, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
    Logger::GetInst().log(Logger::LogLevel::ERROR, __LINE__, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) \
    Logger::GetInst().log(Logger::LogLevel::FATAL, __LINE__, format, ##__VA_ARGS__)

#endif  // _RPC_LOGGER_H