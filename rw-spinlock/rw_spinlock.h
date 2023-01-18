#pragma once

#include <atomic>

struct RWSpinLock {
    void LockRead() {
        auto flag = (flag_.load() >> 1) << 1;
        while (!flag_.compare_exchange_weak(flag, flag + 2)) {
            flag = (flag_.load() >> 1) << 1;
        }
    }

    void UnlockRead() {
        auto flag = flag_.load();
        while (!flag_.compare_exchange_weak(flag, flag - 2)) {
        }
    }

    void LockWrite() {
        int flag = 0;
        while (!flag_.compare_exchange_weak(flag, flag + 1)) {
            flag = 0;
        }
    }

    void UnlockWrite() {
        auto flag = flag_.load();
        while (!flag_.compare_exchange_weak(flag, flag - 1)) {
        }
    }

    std::atomic<int> flag_;
};
