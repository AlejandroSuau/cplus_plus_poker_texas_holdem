#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <format>
#include <string_view>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    template <typename... Args>
    static void Log(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
        std::cout << "[" << CurrentTime() << "] "
                  << "[" << ToString(level) << "] "
                  << std::format(fmt, std::forward<Args>(args)...) << std::endl;
    }

    template <typename... Args>
    static void Info(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Warning(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::WARNING, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Error(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Debug(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }

private:
    static std::string CurrentTime() {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%H:%M:S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    static std::string ToString(LogLevel level) {
        switch(level) {
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::DEBUG:   return "DEBUG";
            default:                return "UNKNOWN LEVEL";
        }
    } 

};