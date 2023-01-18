#pragma once
#include <vector>

template <class Iter, class VoidFoo, class Predicate>
void TransformIf(Iter begin, Iter end, Predicate p, VoidFoo f) {
    std::vector<std::pair<Iter, typename std::iterator_traits<Iter>::value_type>> log;
    for (Iter it = begin; it != end; ++it) {
        try {
            if (p(*it)) {
                try {
                    log.emplace_back(it, *it);
                } catch (...) {
                }
                f(*it);
            }
        } catch (...) {
            for (auto& pair : log) {
                *pair.first = pair.second;
            }
            throw;
        }
    }
}