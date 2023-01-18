#pragma once

constexpr bool IsPrime(int guess) {
    for (int i = 2; i * i <= guess; ++i) {
        if (guess % i == 0) {
            return false;
        }
    }
    return true;
}

constexpr int next_prime(int x) {
    if (x == 1) {
        return 2;
    }
    while (!IsPrime(x)) {
        ++x;
    }
    return x;
}
