#pragma once

#include <string>
#include <cstring>

class StringView {
public:
    StringView(const std::string& str, size_t begin = 0, size_t length = std::string::npos) {
        start_ = &str[begin];
        if (length == std::string::npos || begin + length >= str.size()) {
            length_ = str.size() - begin;
        } else {
            length_ = length;
        }
    }

    StringView(const char* str_p) {
        start_ = str_p;
        length_ = std::strlen(str_p);
    }

    StringView(const char* str_p, size_t length) {
        start_ = str_p;
        length_ = length;
    }

    size_t Size() const {
        return length_;
    }

    const char& operator[](size_t index) const {
        return *(start_ + index);
    }

private:
    const char* start_;
    size_t length_;
};
