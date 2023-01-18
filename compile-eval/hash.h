#pragma once
#include "another_pow.h"
#include <cstring>

constexpr int hash(const char *s, long p, long mod) {
    return *s ? (*s + p * hash(s + 1, p, mod)) % mod : 0;
}
