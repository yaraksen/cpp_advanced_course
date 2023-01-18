#include "lru_cache.h"

LruCache::LruCache(size_t max_size) {
    size_ = max_size;
}

void LruCache::Set(const std::string& key, const std::string& value) {
    if (mp_.count(key)) {
        list_.erase(mp_[key]);
    }
    mp_[key] = list_.insert(list_.end(), std::make_pair(key, value));
    if (list_.size() > size_) {
        mp_.erase(list_.front().first);
        list_.pop_front();
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    if (mp_.count(key)) {
        auto temp = *mp_[key];
        list_.erase(mp_[key]);
        mp_[key] = list_.insert(list_.end(), temp);
        *value = mp_[key]->second;
        return true;
    } else {
        return false;
    }
}
