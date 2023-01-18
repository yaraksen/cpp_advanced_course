#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> out;
    for (size_t i = 0; i < items.size(); ++i) {
        std::unique_ptr<string> p = std::make_unique<string>(*items[i]);
        out.push_back(std::move(p));
    }
    return out;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::unordered_map<string, std::shared_ptr<string>> mp;
    std::vector<std::shared_ptr<string>> out;

    for (size_t i = 0; i < items.size(); ++i) {
        if (!mp.count(*items[i])) {
            mp[*items[i]] = std::make_shared<string>(*items[i]);
        }
        out.push_back(mp[*items[i]]);
    }
    return out;
}
