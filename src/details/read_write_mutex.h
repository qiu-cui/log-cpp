// Copyright (C) 苏州万店掌网络科技有限公司 - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by 崔秋松 <cuiqiusong@ovopark.com>, July 2024
#ifndef LOGCPP_SRC_DETAILS_READ_WRITE_MUTEX_H
#define LOGCPP_SRC_DETAILS_READ_WRITE_MUTEX_H
#ifndef PCH
#    include <condition_variable>
#endif

namespace stdext {
class read_write_mutex {
   public:
    using lock_t = std::unique_lock<std::mutex>;

    void lock_read() {
        lock_t lock(_mutex);
        _condition.wait(lock, [this]() { return !_writting; });
        ++_readers;
    }

    void unlock_read() {
        lock_t lock(_mutex);
        --_readers;
        if (_readers == 0u) { _condition.notify_all(); }
    }

    void lock_write() {
        lock_t lock(_mutex);
        _condition.wait(lock, [this]() { return (_readers == 0u) && !_writting; });
        _writting = true;
    }

    void unlock_write() {
        lock_t lock(_mutex);
        _writting = false;
        _condition.notify_all();
    }

   protected:
    std::condition_variable _condition{};
    std::mutex _mutex{};
    std::uint16_t _readers{ 0u };
    bool _writting{ false };
};

template <typename Mutex>
class read_lock {
   public:
    read_lock(Mutex &mutex) : _mutex(mutex) { mutex.lock_read(); }
    ~read_lock() { _mutex.unlock_read(); }

   private:
    Mutex &_mutex;
};

template <typename Mutex>
class write_lock {
   public:
    write_lock(Mutex &mutex) : _mutex(mutex) { mutex.lock_write(); }
    ~write_lock() { _mutex.unlock_write(); }

   private:
    Mutex &_mutex;
};

}  // namespace stdext

#endif  // LOGCPP_SRC_DETAILS_READ_WRITE_MUTEX_H