#include "is_prime.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <atomic>

std::atomic<bool> ans;

void Worker(uint64_t x, uint64_t from, uint64_t to) {
    if (!ans) {
        return;
    }
    for (auto i = from; i < to; ++i) {
        if (x % i == 0) {
            ans = false;
            return;
        }
    }
}

bool IsPrime(uint64_t x) {
    if (x <= 1) {
        return false;
    }
    ans = true;
    uint64_t root = sqrt(x);
    uint64_t num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;
    uint64_t interval_size = std::ceil(sqrt(x) / double(num_threads));
    for (uint64_t i = 2; i <= root; i += interval_size) {
        workers.emplace_back(Worker, x, i, i + interval_size);
    }
    for (auto& t : workers) {
        t.join();
    }
    return ans;
}
