#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }

    void Leave() {
        auto lock = std::lock_guard{mutex_};
        ++count_;
        if (!queue_.empty()) {
            auto p = &queue_.front();
            queue_.pop();
            p->notify_one();
        }
    }

    template <class Func>
    void Enter(Func callback) {
        auto lock = std::unique_lock<std::mutex>{mutex_};
        if (count_ != 0) {
            --count_;
            callback(count_);
            return;
        }
        queue_.emplace();
        while (!count_) {
            queue_.front().wait(lock);
        }
        callback(count_);
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::mutex mutex_;
    // std::condition_variable cv_;
    int count_ = 0;
    std::queue<std::condition_variable> queue_;
};
