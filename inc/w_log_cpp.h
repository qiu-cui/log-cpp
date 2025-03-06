// Copyright (C) 苏州万店掌网络科技有限公司 - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by 崔秋松 <cuiqiusong@ovopark.com>, June 2024

#ifndef LOGCPP_INC_WLOGCPP
#define LOGCPP_INC_WLOGCPP

// #ifndef LOG_CPP_LEVEL
// #    define SPDLOG_ACTIVE_LEVEL w_log_cpp::LOG_LEVEL_E::INFO
// #else
// #    define SPDLOG_ACTIVE_LEVEL LOG_CPP_LEVEL
// #endif

#include <spdlog/spdlog.h>

#include <cstddef>

#if defined(LOGCPP_SHARED_LIB)
#    if defined(_WIN32)
#        ifdef LOGCPP_EXPORTS
#            define LOGCPP_API __declspec(dllexport)
#        else  // !LOGCPP_EXPORTS
#            define LOGCPP_API __declspec(dllimport)
#        endif
#    else  // !defined(_WIN32)
#        define LOGCPP_API __attribute__((visibility("default")))
#    endif
#else  // !defined(LOGCPP_SHARED_LIB)
#    define LOGCPP_API
#endif
#define LOGCPP_INLINE inline


#define LOG_CPP(level, fmt, ...) w_log_cpp::log(level, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);

#define LOG_TRACE(fmt, ...) \
    w_log_cpp::log(w_log_cpp::LOG_LEVEL_E::TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
    w_log_cpp::log(w_log_cpp::LOG_LEVEL_E::DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    w_log_cpp::log(w_log_cpp::LOG_LEVEL_E::INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    w_log_cpp::log(w_log_cpp::LOG_LEVEL_E::WARN, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) \
    w_log_cpp::log(w_log_cpp::LOG_LEVEL_E::ERR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

namespace w_log_cpp {

typedef enum LOG_LEVEL {
    TRACE = spdlog::level::trace,
    DEBUG = spdlog::level::debug,
    INFO = spdlog::level::info,
    WARN = spdlog::level::warn,
    ERR = spdlog::level::err,
    CRITICAL = spdlog::level::critical
} LOG_LEVEL_E;

/**
 * @brief Set the Stdout Enable object
 *
 * @param enable true: enable, false: disable
 */
LOGCPP_API void setStdoutEnable(bool enable);

/**
 * @brief Set the Log Level object
 *
 * @param level
 */
LOGCPP_API void setLogLevel(LOG_LEVEL_E level);

/**
 * @brief Set the Flush On object
 *
 * @param level
 */
LOGCPP_API void setFlushOn(LOG_LEVEL_E level);

/**
 * @brief 设置线程池大小
 *
 * @param queueSize 队列大小
 * @param threadSize 线程数量
 */
LOGCPP_API void setThreadPoolSize(size_t queueSize, size_t threadSize);

/**
 * @brief 初始化日志系统,未使用初始化函数时,默认输出到控制台,不需要控制台输出时，调用setStdoutEnable(false)
 *          需要在调用log函数前调用,否则只默认初始化
 *
 * @param sinks 日志输出目标，可以是控制台、文件、网络等，根据实际情况添加
 *
 */
LOGCPP_API void initLog(spdlog::sinks_init_list sinks);

/**
 * @brief 使用文件sink初始化日志系统
 *
 * @param logfilePath 日志文件路径
 * @param maxFileSize 单个日志文件最大大小
 * @param maxFileNum 日志文件数量
 */
LOGCPP_API void initLogWithFileSink(const char *logfilePath = "./log/log.txt", size_t maxFileSize = 1024 * 1024 * 10,
                                    size_t maxFileNum = 3);

/**
 * @brief 关闭日志系统(避免在windows下出现异常)
 */
LOGCPP_API void shutdow();

/**
 * @brief 日志函数
 */
LOGCPP_API void log(LOG_LEVEL_E, const char *, int, const char *, const char *, ...);

}  // namespace w_log_cpp

#endif  // LOGCPP_INC_WLOGCPP