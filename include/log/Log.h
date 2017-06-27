#pragma once

#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

/**
 * Logging levels
 */
#define DEBUG_LEVEL 1
#define INFO_LEVEL 2
#define WARN_LEVEL 3
#define ERROR_LEVEL 4

/**
 * Default the logging level to DEBUG
 */
#define LOGGING_LEVEL DEBUG_LEVEL
#define LOGGING_OUT std::cout

/**
 * ANSI color codes
 */
#define DEFAULT_LOG_COLOR "\33[0m"
#define BLACK_LOG_COLOR "\33[0;30m"
#define RED_LOG_COLOR "\33[0;31m"
#define GREEN_LOG_COLOR "\33[0;32m"
#define YELLOW_LOG_COLOR "\33[0;33m"
#define BLUE_LOG_COLOR "\33[0;34m"
#define PURPLE_LOG_COLOR "\33[0;35m"
#define CYAN_LOG_COLOR "\33[0;36m"
#define WHITE_LOG_COLOR "\33[0;37m"

/**
 * Logging structs and templates
 */
struct None {
};

template <typename List>
struct LogData {
    List list;
};

template <typename Begin, typename Value>
LogData<std::pair<Begin, const Value&> > operator<<(LogData<Begin> begin, const Value& value)
{
    return { { begin.list, value } };
}

template <typename Begin, size_t n>
LogData<std::pair<Begin, const char*> > operator<<(LogData<Begin> begin, const char (&value)[n])
{
    return { { begin.list, value } };
}

inline void logRecursive(None) {}

template <typename Begin, typename Last>
void logRecursive(const std::pair<Begin, Last>& data)
{
    logRecursive(data.first);
    LOGGING_OUT << data.second;
}

template <typename List>
void log(const char* level, const char* file, const char* time, int32_t line, const LogData<List>& data)
{
    LOGGING_OUT << BLACK_LOG_COLOR;
    LOGGING_OUT << "[ " << time << " ] ";
    LOGGING_OUT << level << " ";
    LOGGING_OUT << DEFAULT_LOG_COLOR;
    logRecursive(data.list);
    LOGGING_OUT << " ";
    LOGGING_OUT << BLUE_LOG_COLOR;
    LOGGING_OUT << "(" << file << ":" << line << ")";
    LOGGING_OUT << std::endl;
}

std::string getTime();

/**
 * Logging level macros
 */
#define DEBUG_STR (BLUE_LOG_COLOR "[ DEBUG ]")
#define INFO_STR (GREEN_LOG_COLOR "[ INFO ]")
#define WARN_STR (YELLOW_LOG_COLOR "[ WARN ]")
#define ERROR_STR (RED_LOG_COLOR "[ ERROR ]")

/**
 * Filename macro
 */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/**
 * Logging macros based on logging level
 */
#if LOGGING_LEVEL <= DEBUG_LEVEL
#define LOG_DEBUG(x) (log(DEBUG_STR, __FILENAME__, getTime().c_str(), __LINE__, LogData<None>() << x))
#else
#define LOG_DEBUG(x)
#endif

#if LOGGING_LEVEL <= INFO_LEVEL
#define LOG_INFO(x) (log(INFO_STR, __FILENAME__, getTime().c_str(), __LINE__, LogData<None>() << x))
#else
#define LOG_INFO(x)
#endif

#if LOGGING_LEVEL <= WARN_LEVEL
#define LOG_WARN(x) (log(WARN_STR, __FILENAME__, getTime().c_str(), __LINE__, LogData<None>() << x))
#else
#define LOG_WARN(x)
#endif

#if LOGGING_LEVEL <= ERROR_LEVEL
#define LOG_ERROR(x) (log(ERROR_STR, __FILENAME__, getTime().c_str(), __LINE__, LogData<None>() << x))
#else
#define LOG_ERROR(x)
#endif
