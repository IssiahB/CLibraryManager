#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <spdlog/spdlog.h>
#include <memory>

class Logger {
public:
    static void init();  // Initialize loggers
    static std::shared_ptr<spdlog::logger> dbLogger;   // Database logs
    static std::shared_ptr<spdlog::logger> userLogger; // User-related logs
    static std::shared_ptr<spdlog::logger> systemLogger; // General system logs
};

#endif // LOGGER_HPP
