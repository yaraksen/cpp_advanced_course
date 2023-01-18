#pragma once

#include <utility>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <queue>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : free_space_(size) {
    }

    void Send(const T& value) {
        auto lock = std::unique_lock<std::mutex>{mutex_};
        if (closed_) {
            throw std::runtime_error("Cannot send through closed channel");
        }
        while (!free_space_ && !closed_) {
            not_full_.wait(lock);
        }
        if (closed_) {
            throw std::runtime_error("Cannot send through closed channel");
        }
        --free_space_;
        buffer_.push(std::move(value));
        not_empty_.notify_all();
    }

    std::optional<T> Recv() {
        auto lock = std::unique_lock<std::mutex>{mutex_};
        /*if (closed_ && buffer_.empty()) {
            not_full_.notify_all();
            return std::nullopt;
        }*/
        /*if (buffer_.empty()) {
            return std::nullopt;
        }*/
        while (!closed_ && buffer_.empty()) {
            not_empty_.wait(lock);
        }
        if (buffer_.empty()) {
            not_full_.notify_all();
            return std::nullopt;
        }
        auto tmp = std::move(buffer_.front());
        buffer_.pop();
        ++free_space_;
        not_full_.notify_all();
        return tmp;
    }

    void Close() {
        auto lock = std::unique_lock<std::mutex>{mutex_};
        closed_ = true;
        not_empty_.notify_all();
    }

private:
    std::mutex mutex_;
    bool closed_ = false;
    int free_space_ = 0;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    std::queue<T> buffer_;
};
