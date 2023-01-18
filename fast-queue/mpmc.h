#pragma once

#include <mutex>
#include <queue>
#include <atomic>
#include <array>

template <class T>
struct Node {
    /*Node(T val, std::atomic<size_t> gen) {
        value = val;
        generation.store(gen);
    };*/
    std::atomic<size_t> generation;
    T value;
};

template <class T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(int size) : cycle_(size), max_size_(size) {
        for (size_t i = 0; i < max_size_; ++i) {
            cycle_[i].generation = i;
        }
    }

    /*
     * [(x, 0), (x, 1), ... (x, 3)] max_size_ - 1 = 3 = 0b11
     */

    bool Enqueue(const T& value) {
        while (true) {
            auto h_index = h_.load();
            if (max_size_ + t_.load() == h_index) {
                return false;
            }
            auto h_index_mod = (max_size_ - 1) & h_index;  // by module of 2^k
            if (h_index == cycle_[h_index_mod].generation.load() &&
                h_.compare_exchange_weak(h_index, h_index + 1)) {
                cycle_[h_index_mod].value = value;
                ++cycle_[h_index_mod].generation;
                return true;
                // h_index_mod = (max_size_ - 1) & h_index; // ???
            }
        }
    }

    bool Dequeue(T& data) {
        while (true) {
            auto t_index = t_.load();
            if (h_.load() == t_index) {
                return false;
            }
            auto t_index_mod = (max_size_ - 1) & t_index;  // by module of 2^k
            if (t_index + 1 == cycle_[t_index_mod].generation.load() &&
                t_.compare_exchange_weak(t_index, t_index + 1)) {
                data = cycle_[t_index_mod].value;
                cycle_[t_index_mod].generation = t_index + max_size_;
                return true;
                // t_index_mod = (max_size_ - 1) & t_index; // ???
            }
        }
    }

private:
    // std::queue<T> queue_;
    // std::mutex mutex_;
    std::vector<Node<T>> cycle_;
    std::atomic<size_t> h_ = 0;  // head index
    std::atomic<size_t> t_ = 0;  // tail index
    size_t max_size_ = 0;
};