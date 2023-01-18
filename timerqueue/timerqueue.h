#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <set>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

public:
    void Add(const T& item, TimePoint at) {
        auto guard = std::lock_guard{mutex_};
        mp_.insert(std::make_pair(at, item));
        time_came_.notify_one();
    }

    T Pop() {
        auto guard = std::unique_lock{mutex_};
        while (mp_.empty()) {
            time_came_.wait(guard);
        }
        time_came_.wait_until(guard, mp_.begin()->first);
        T result = std::move(mp_.begin()->second);
        mp_.erase(mp_.begin());
        return result;
    }

private:
    std::set<std::pair<TimePoint, T>> mp_;
    std::mutex mutex_;
    std::condition_variable time_came_;
};
