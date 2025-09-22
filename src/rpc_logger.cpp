#include "rpc_logger.h"

#include <fmt/core.h>

#include <ctime>
#include <filesystem>

Logger::~Logger() {
    stop_ = true;
    if (processThread_.joinable()) {
        processThread_.join();
    }
    file_.close_file();
}

std::string Logger::genDefaultLogFileName() {
    // get current timestamp
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S") << ".txt";
    std::string timestampStr = ss.str();

    // get current executable path
    std::filesystem::path exePath =
        std::filesystem::canonical("/proc/self/exe").parent_path();  // Linux

    // return the log file path
    return (exePath / "log" / timestampStr).string();
}

void Logger::init(LogLevel level, const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (processThread_.joinable()) {
        processThread_.join();
    }

    if (filename.empty()) {
        file_.open_file(genDefaultLogFileName());
    } else {
        file_.open_file(filename);
    }

    level_ = level;

    // Start the log processing thread
    processThread_ = std::thread([this] { processLogTasks(); });
}

void Logger::processLogTasks() {
    while (true) {
        if (stop_ && taskQueue_.empty()) {
            break;
        }
        std::function<void()> task{nullptr};
        if (!taskQueue_.empty()) {
            taskQueue_.pop(task);
            if (task) task();
        }
    }
}
