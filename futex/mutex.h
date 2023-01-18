#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <mutex>
#include <atomic>

// Atomically do the following:
//    if (*value == expected_value) {
//        sleep_on_address(value)
//    }
void FutexWait(int *value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expected_value, nullptr, nullptr, 0);
}

// Wakeup 'count' threads sleeping on address of value(-1 wakes all)
void FutexWake(int *value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

int Cmpxchg(std::atomic<int> &val, int expected, int desired) {
    val.compare_exchange_strong(expected, desired);
    return expected;
}

class Mutex {
public:
    Mutex() : val_(0) {
    }

    void Lock() {
        int counter;
        if ((counter = Cmpxchg(val_, 0, 1)) != 0) {
            if (counter != 2) {
                counter = val_.exchange(2);
            }
            while (counter) {
                FutexWait(reinterpret_cast<int *>(&val_), 2);
                counter = val_.exchange(2);
            }
        }
    }

    void Unlock() {
        if (val_.fetch_sub(1) != 1) {
            val_.store(0);
            FutexWake(reinterpret_cast<int *>(&val_), 1);
        }
    }

private:
    std::atomic<int> val_;
};
