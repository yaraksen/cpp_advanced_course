#pragma once
#include <mutex>
#include <thread>
#include <cmath>
#include <vector>
#include <atomic>

std::mutex place_mutex;

template <class RandomAccessIterator, class T, class Func>
void Worker(RandomAccessIterator first, RandomAccessIterator last, std::vector<T>& place,
            Func func) {
    if (first == last) {
        return;
    }
    T cur_value(*first);
    first++;
    while (first != last) {
        cur_value = func(cur_value, *first++);
    }
    const std::lock_guard<std::mutex> lock(place_mutex);
    place.push_back(cur_value);
}

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    if (first == last) {
        return initial_value;
    }
    std::vector<std::thread> workers;
    uint64_t n = std::distance(first, last);
    if (n == 1) {
        return func(initial_value, *first);
    }
    uint64_t threads_num = std::thread::hardware_concurrency();
    uint64_t interval_size = std::ceil(double(n) / double(threads_num));
    uint64_t offset = 0;
    std::vector<T> answers;
    for (uint64_t i = 0; i < threads_num; ++i) {
        if (offset > n) {
            break;
        }
        workers.emplace_back(Worker<RandomAccessIterator, T, Func>, first + offset,
                             first + std::min(offset + interval_size, n), std::ref(answers), func);
        offset += interval_size;
    }
    for (std::thread& t : workers) {
        t.join();
    }
    auto cur_value(initial_value);
    auto it1 = answers.begin();
    auto it2 = answers.end();
    while (it1 != it2) {
        cur_value = func(cur_value, *it1++);
    }
    return cur_value;
}
