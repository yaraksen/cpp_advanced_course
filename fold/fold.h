#pragma once
#include <vector>

struct Sum {
    int operator()(int a, int b) const {
        return a + b;
    }
};

struct Prod {
    int operator()(int a, int b) const {
        return a * b;
    }
};

struct Concat {
    std::vector<int> operator()(const std::vector<int>& a, const std::vector<int>& b) {
        std::vector<int> all;
        all.reserve(a.size() + b.size());
        all.insert(all.end(), a.begin(), a.end());
        all.insert(all.end(), b.begin(), b.end());
        return all;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    for (Iterator it = first; it != last; ++it) {
        init = func(init, *it);
    }
    return init;
}

class Length {
public:
    Length(int* pointer) : pointer_(pointer) {
    }
    template <typename T>
    T operator()(T a [[maybe_unused]], T b [[maybe_unused]]) {
        ++*pointer_;
        return T{};
    }

private:
    int* pointer_;
};
