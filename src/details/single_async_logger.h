// Copyright (C) 苏州万店掌网络科技有限公司 - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by 崔秋松 <cuiqiusong@ovopark.com>, June 2024

#ifndef LOGCPP_SRC_DETAILS_SINGLE_ASYNC_LOGGER_H
#define LOGCPP_SRC_DETAILS_SINGLE_ASYNC_LOGGER_H

#include <spdlog/async_logger.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <shared_mutex>

#include "details/read_write_mutex.h"
#include "details/singleton.h"

namespace w_log_cpp {
namespace details {

class SingleAsyncLogger : public w_log_cpp::details::Singleton<SingleAsyncLogger> {
    using async_logger_t = spdlog::async_logger;
    using stdout_color_sink_mt_t = spdlog::sinks::stdout_color_sink_mt;

    friend class w_log_cpp::details::Singleton<SingleAsyncLogger>;

    explicit SingleAsyncLogger(uint32_t queueSize = 10, uint32_t threadSize = 1);

   public:
    SingleAsyncLogger(const SingleAsyncLogger &) = delete;
    SingleAsyncLogger &operator=(const SingleAsyncLogger &) = delete;
    ~SingleAsyncLogger();

    void init(spdlog::sinks_init_list sinks);
    void setLogLevel(spdlog::level::level_enum level);
    void setFlushOn(spdlog::level::level_enum level);
    void setStdoutEnabled(bool enabled);
    void log(spdlog::source_loc &&source, spdlog::level::level_enum lvl, const char *msg) {
        stdext::read_lock<stdext::read_write_mutex> lock(rwmutex);
        if (logger)
            logger->log(source, lvl, msg);
        else
            spdlog::log(source, lvl, msg);
    }

    void shutdown();

   private:
    std::shared_ptr<async_logger_t> logger{ nullptr };
    std::shared_ptr<stdout_color_sink_mt_t> stdout_sink{ std::make_shared<stdout_color_sink_mt_t>() };

    std::once_flag init_flag{};
    stdext::read_write_mutex rwmutex{};
};
}  // namespace details
}  // namespace w_log_cpp

#endif  // LOGCPP_SRC_DETAILS_SINGLE_ASYNC_LOGGER_H