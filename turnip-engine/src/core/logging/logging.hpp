#pragma once
#include "defines.hpp"
#include "memory/memory.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace tur
{
	inline tur_shared<spdlog::logger> MainLogger;
	inline tur_shared<spdlog::logger> InfoLogger;

	inline void initialize_logging_system()
	{
		std::vector<spdlog::sink_ptr> sinks;

		std::string logFilename = TUR_PROJECT_NAME;
		logFilename.append(".log");

		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilename, true));

		sinks[0]->set_pattern("[%T] [%n] [%^%l%$]: %v");
		sinks[1]->set_pattern("[%D | %T] [%n] [%l]: %v");

		// Core Logger:
		MainLogger = tur::make_shared<spdlog::logger>(TUR_PROJECT_NAME, std::begin(sinks), std::end(sinks));
		spdlog::register_logger(MainLogger);

		MainLogger->set_level(spdlog::level::trace);
		MainLogger->flush_on(spdlog::level::trace);

		// Info Logger:
		InfoLogger = tur::make_shared<spdlog::logger>("Info", sinks[0]);
		spdlog::register_logger(InfoLogger);

		InfoLogger->set_level(spdlog::level::trace);
		InfoLogger->flush_on(spdlog::level::trace);
	}
}

#ifdef TUR_DEBUG
	#define TUR_LOG_CRITICAL(...)                                                                                      \
		{                                                                                                              \
			::tur::MainLogger->critical(__VA_ARGS__);                                                                  \
			TUR_BREAKPOINT();                                                                                          \
		}
	#define TUR_LOG_ERROR(...) ::tur::MainLogger->error(__VA_ARGS__)
	#define TUR_LOG_WARN(...) ::tur::MainLogger->warn(__VA_ARGS__)
	#define TUR_LOG_DEBUG(...) ::tur::MainLogger->debug(__VA_ARGS__)
	#define TUR_LOG_TRACE(...) ::tur::InfoLogger->trace(__VA_ARGS__)
	#define TUR_LOG_INFO(...) ::tur::InfoLogger->info(__VA_ARGS__)

#else
	#define TUR_LOG_CRITICAL(...)                                                                                      \
		{                                                                                                              \
			::tur::MainLogger->critical(__VA_ARGS__);                                                                  \
			TUR_BREAKPOINT();                                                                                          \
		}
	#define TUR_LOG_ERROR(...) ::tur::MainLogger->error(__VA_ARGS__)
	#define TUR_LOG_WARN(...)                                                                                          \
		{                                                                                                              \
		}
	#define TUR_LOG_DEBUG(...)                                                                                         \
		{                                                                                                              \
		}
	#define TUR_LOG_TRACE(...)                                                                                         \
		{                                                                                                              \
		}
	#define TUR_LOG_INFO(...)                                                                                          \
		{                                                                                                              \
		}
#endif