// Copyright (C) 苏州万店掌网络科技有限公司 - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by 崔秋松 <cuiqiusong@ovopark.com>, June 2024
#include "single_async_logger.h"

#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <mutex>

#include "read_write_mutex.h"

namespace w_log_cpp {
namespace details {
namespace {
const std::string LOG_CPP_PATTERN{ "[%Y-%m-%d %H:%M:%S.%e] %^[%L][%s:%#,%t] %v%$" };

const std::string LOG_CPP_LOGGER_NAME{ "w_log_cpp" };

const spdlog::level::level_enum LOG_CPP_FLUSH_ON{ spdlog::level::info };
}  // namespace

SingleAsyncLogger::SingleAsyncLogger(uint32_t queueSize, uint32_t threadSize) {
    spdlog::init_thread_pool(queueSize, threadSize);
    auto logger_tmp = std::make_shared<spdlog::async_logger>(LOG_CPP_LOGGER_NAME, stdout_sink, spdlog::thread_pool());
    logger_tmp->set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
    logger_tmp->set_pattern(LOG_CPP_PATTERN);
    logger_tmp->flush_on(LOG_CPP_FLUSH_ON);
    stdext::write_lock<stdext::read_write_mutex> lock(rwmutex);
    logger.swap(logger_tmp);
    // spdlog::set_default_logger(logger);
}

SingleAsyncLogger::~SingleAsyncLogger() {
    if (logger) shutdown();
}

void SingleAsyncLogger::init(spdlog::sinks_init_list sinks) {
    std::call_once(init_flag, [=] {
        std::vector<spdlog::sink_ptr> sink_list;

        if (stdout_sink) sink_list.push_back(stdout_sink);

        for (auto &sink : sinks) { sink_list.push_back(sink); }

        auto new_logger = std::make_shared<spdlog::async_logger>(LOG_CPP_LOGGER_NAME, std::begin(sink_list),
                                                                 std::end(sink_list), spdlog::thread_pool());
        new_logger->set_level(logger->level());
        new_logger->set_pattern(LOG_CPP_PATTERN);
        new_logger->flush_on(LOG_CPP_FLUSH_ON);

        // logger->swap(*new_logger);
        stdext::write_lock<stdext::read_write_mutex> lock(rwmutex);
        logger.swap(new_logger);
        // spdlog::set_default_logger(logger);
    });
}

void SingleAsyncLogger::setLogLevel(spdlog::level::level_enum level) {
    stdext::write_lock<stdext::read_write_mutex> lock(rwmutex);
    if (logger) { logger->set_level(level); }
}

void SingleAsyncLogger::setFlushOn(spdlog::level::level_enum level) {
    stdext::write_lock<stdext::read_write_mutex> lock(rwmutex);
    if (logger) logger->flush_on(level);
}

void SingleAsyncLogger::setStdoutEnabled(bool enabled) {
    if (stdout_sink) {
        if (enabled)
            stdout_sink->set_level(logger->level());
        else
            stdout_sink->set_level(spdlog::level::off);
    }
}

void SingleAsyncLogger::shutdown() {
    if (logger) {
        stdext::write_lock<stdext::read_write_mutex> lock(rwmutex);
        logger->flush();
        logger.reset();
    }
    spdlog::set_level(spdlog::level::off);
    spdlog::shutdown();
}

}  // namespace details
}  // namespace w_log_cpp