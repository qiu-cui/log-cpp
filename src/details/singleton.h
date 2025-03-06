// Copyright (C) 苏州万店掌网络科技有限公司 - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by 崔秋松 <cuiqiusong@ovopark.com>, June 2024

#ifndef LOGCPP_SRC_DETAILS_SINGLETON_H
#define LOGCPP_SRC_DETAILS_SINGLETON_H

#include <memory>
#include <mutex>

namespace w_log_cpp {
namespace details {
/// \brief 单例模板类
/// \tparam T
template <typename T>
class Singleton {
   protected:
    Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    static std::shared_ptr<T> _instance;
    static std::once_flag _flag;

   public:
    // 参数模板
    template <typename... Args>
    static std::shared_ptr<T> GetInstance(Args &&...args) {
        std::call_once(
            _flag, [&](Args &&...args) { _instance = std::shared_ptr<T>(new T(std::forward<Args>(args)...)); },
            std::forward<Args>(args)...);

        return _instance;
    }

    ~Singleton() {}
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

template <typename T>
std::once_flag Singleton<T>::_flag;
}  // namespace details
}  // namespace w_log_cpp

#endif  // LOGCPP_SRC_DETAILS_SINGLETON_H