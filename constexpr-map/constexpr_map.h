#pragma once
#include <array>
#include <stdexcept>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        if (index_ == MaxSize - 1) {
            throw std::logic_error("Key exists!");
        } else {
            for (int i = 0; i <= index_; ++i) {
                if (map_[i].first == key) {
                    return map_[i].second;
                }
            }
            ++index_;
            map_[index_].first = key;
            return map_[index_].second;
        }
    }

    constexpr const V& operator[](const K& key) const {
        for (int i = 0; i <= index_; ++i) {
            if (map_[i].first == key) {
                return map_[i].second;
            }
        }
        throw std::logic_error("No such key!");
    }

    constexpr bool Erase(const K& key) {
        for (int i = 0; i <= index_; ++i) {
            if (map_[i].first == key) {
                for (int j = i + 1; j <= index_; ++j) {
                    Swap(map_[i], map_[j]);
                }
                --index_;
                return true;
            }
        }
        return false;
    }

    constexpr bool Find(const K& key) const {
        for (int i = 0; i <= index_; ++i) {
            if (map_[i].first == key) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return index_ + 1;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return map_[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return map_[pos];
    }

private:
    int index_ = -1;
    std::array<std::pair<K, V>, MaxSize> map_;
};
