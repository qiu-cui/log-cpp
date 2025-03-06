#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/sqlite_sink.h>

#include <chrono>
#include <memory>
#include <thread>

#include "w_log_cpp.h"

int main() {
    // initlog在
    /*w_log_cpp::initLogWithFileSink();
    spdlog::sinks::sqlite_sink_mt sqlite_sink("./log/log.db");*/

    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>("./log/log.txt", 1024 * 1024 * 10, 3);
    std::shared_ptr<spdlog::sinks::sqlite_sink_mt> sqlite_sink =
        std::make_shared<spdlog::sinks::sqlite_sink_mt>("./log/log.db");
    w_log_cpp::initLog({ file_sink, sqlite_sink });

    LOG_DEBUG("hello debug");
    LOG_INFO("hello info");
    w_log_cpp::setLogLevel(w_log_cpp::LOG_LEVEL_E::DEBUG);
    LOG_DEBUG("hello debug");

    const auto func = [] {
        LOG_TRACE("hello trace");
        LOG_WARN("hello warn");
    };

    func();
    // w_log_cpp::setStdoutEnable(false);

    std::thread t([] {
        // sleep 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));
        LOG_INFO("hello %s", "123");
        LOG_DEBUG("hello debug");
        LOG_INFO("hello info");
        LOG_TRACE("hello trace");
        LOG_WARN("hello warn");
    });

    std::thread t2([] {
        // sleep 1s
        std::this_thread::sleep_for(std::chrono::milliseconds(999));
        LOG_DEBUG("hello %s", "123");
        LOG_DEBUG("hello debug");
        LOG_INFO("hello info");
        LOG_TRACE("hello trace");
        LOG_WARN("hello warn");
    });
    char *buf = new char[2025]{ 0 };
    for (int i = 0; i < 2024; i++) { buf[i] = 'a' + i % 26; }

    t.join();
    t2.detach();
    LOG_INFO("%s", buf);

    w_log_cpp::shutdow();
    return 0;
}