#include "Logger.hpp"
#include <iostream>
#include <spdlog/sinks/rotating_file_sink.h>

// Define static members
std::shared_ptr<spdlog::logger> Logger::dbLogger;
std::shared_ptr<spdlog::logger> Logger::userLogger;
std::shared_ptr<spdlog::logger> Logger::systemLogger;

void Logger::init()
{
    try
    {
        // Create rotating file sinks
        auto dbSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/database.log", 1048576 * 5, 3);
        auto userSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/user.log", 1048576 * 5, 3);
        auto systemSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/system.log", 1048576 * 5, 3);

        // Create loggers
        dbLogger = std::make_shared<spdlog::logger>("database_logger", dbSink);
        userLogger = std::make_shared<spdlog::logger>("user_logger", userSink);
        systemLogger = std::make_shared<spdlog::logger>("system_logger", systemSink);

        // Set log levels
        dbLogger->set_level(spdlog::level::info);
        userLogger->set_level(spdlog::level::info);
        systemLogger->set_level(spdlog::level::debug);

        // Register loggers globally
        spdlog::register_logger(dbLogger);
        spdlog::register_logger(userLogger);
        spdlog::register_logger(systemLogger);

        spdlog::info("Logging initialized successfully.");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}
