#pragma once

#include <constexpr_map.h>

#include <type_traits>

template <class K, class V>
constexpr void Swap(std::pair<K, V>& a, std::pair<K, V>& b) {
    auto p = a;
    a.first = b.first;
    a.second = b.second;
    b.first = p.first;
    b.second = p.second;
}

template <class K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {
    ConstexprMap<K, V, S> map2(map);
    for (int _ = 0; _ < map2.Size(); ++_) {
        for (int i = 0; i < map2.Size() - 1; ++i) {
            if constexpr (std::is_integral_v<K>) {
                if (map2.GetByIndex(i).first < map2.GetByIndex(i + 1).first) {
                    Swap(map2.GetByIndex(i), map2.GetByIndex(i + 1));
                }
            } else {
                if (!(map2.GetByIndex(i).first < map2.GetByIndex(i + 1).first)) {
                    Swap(map2.GetByIndex(i), map2.GetByIndex(i + 1));
                }
            }
        }
    }
    return map2;
}