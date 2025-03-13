// Copyright (C) 苏州万店掌网络科技有限公司 - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by 崔秋松 <cuiqiusong@ovopark.com>, June 2024

#include "w_log_cpp.h"

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <cstdarg>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

#include "details/single_async_logger.h"

namespace w_log_cpp {
namespace {
std::once_flag logger_flag{};

size_t QueueSize = 10;
size_t ThreadSize = 1;

void callonce_init(std::function<void(spdlog::sinks_init_list sinks)> &&func = nullptr,
                   spdlog::sinks_init_list &&sinks = {}) {
    std::call_once(logger_flag, [] { details::SingleAsyncLogger::GetInstance(QueueSize, ThreadSize); });
    if (func) func(sinks);
}

}  // namespace

LOGCPP_API void log(LOG_LEVEL_E level, const char *file, int line, const char *func, const char *fmt, ...) {
    callonce_init();

    va_list args;
    va_start(args, fmt);
    std::unique_ptr<std::string> pbuf(new std::string(1024, '\0'));
    int ret = 0;
    while ((ret = vsnprintf(&(*pbuf)[0], pbuf->size()-1, fmt, args)) >= static_cast<int>(pbuf->size())) {
        pbuf->resize(pbuf->size() * 2);
        //va_start(args, fmt);
    }
    if (ret < 0) {
        std::cerr << "vsnprintf failed\n";
        va_end(args);
        return;
    }
    try {
        details::SingleAsyncLogger::GetInstance()->log(spdlog::source_loc{ file, line, func },
                                                       static_cast<spdlog::level::level_enum>(level), pbuf->c_str());
    } catch (const std::exception &e) { std::cerr << e.what() << '\n'; }
    va_end(args);
}

LOGCPP_API void setStdoutEnable(bool enable) { details::SingleAsyncLogger::GetInstance()->setStdoutEnabled(enable); }

LOGCPP_API void setLogLevel(LOG_LEVEL_E level) {
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
    details::SingleAsyncLogger::GetInstance()->setLogLevel(static_cast<spdlog::level::level_enum>(level));
}

LOGCPP_API void setFlushOn(LOG_LEVEL_E level) {
    spdlog::flush_on(static_cast<spdlog::level::level_enum>(level));
    details::SingleAsyncLogger::GetInstance()->setFlushOn(static_cast<spdlog::level::level_enum>(level));
}

LOGCPP_API void setThreadPoolSize(size_t queueSize, size_t threadSize) {
    QueueSize = queueSize;
    ThreadSize = threadSize;
}

LOGCPP_API void initLog(spdlog::sinks_init_list sinks) {
    callonce_init([&](spdlog::sinks_init_list sinks) { details::SingleAsyncLogger::GetInstance()->init(sinks); },
                  std::move(sinks));
}

LOGCPP_API void initLogWithFileSink(const char *logfilePath, size_t maxFileSize, size_t maxFileNum) {
    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logfilePath, maxFileSize, maxFileNum);
    initLog({ file_sink });
}

LOGCPP_API void shutdow() { details::SingleAsyncLogger::GetInstance()->shutdown(); }

}  // namespace w_log_cpp
