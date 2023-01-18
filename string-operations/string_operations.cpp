#include "string_operations.h"
#include <string_view>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <cctype>
#include <vector>
#include <memory>
#include <sstream>
#include <iterator>
#include <cstddef>
#include <string.h>

bool StartsWith(std::string_view string, std::string_view text) {
    size_t i = 0, j = 0;
    size_t s1 = string.size(), s2 = text.size();
    if (s1 < s2) {
        return false;
    }
    while (i < s1 && j < s2) {
        if (string[i] != text[j]) {
            return false;
        }
        ++i, ++j;
    }
    return true;
}

bool EndsWith(std::string_view string, std::string_view text) {
    int i = string.size() - 1, j = text.size() - 1;
    if (i < j) {
        return false;
    }
    while (i >= 0 && j >= 0) {
        if (string[i] != text[j]) {
            return false;
        }
        --i, --j;
    }
    return true;
}

std::string_view StripPrefix(std::string_view string, std::string_view prefix) {
    size_t i = 0, j = 0;
    size_t s1 = string.size(), s2 = prefix.size();
    if (s1 < s2) {
        return string;
    }
    while (i < s1 && j < s2) {
        if (string[i] != prefix[j]) {
            return string;
        }
        ++i, ++j;
    }
    return string.substr(i, s1 - i);
}

std::string_view StripSuffix(std::string_view string, std::string_view prefix) {
    int i = string.size() - 1, j = prefix.size() - 1;
    if (i < j) {
        return string;
    }
    while (i >= 0 && j >= 0) {
        if (string[i] != prefix[j]) {
            return string;
        }
        --i, --j;
    }
    return string.substr(0, i + 1);
}

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n) {
    if (pos + n > s.size()) {
        return s;
    } else {
        return s.substr(pos, n);
    }
}

std::string_view StripAsciiWhitespace(std::string_view string) {
    size_t i;
    for (i = 0; i < string.size(); ++i) {
        if (!std::isspace(string[i])) {
            break;
        }
    }
    int j;
    for (j = string.size() - 1; j >= 0; --j) {
        if (!std::isspace(string[j])) {
            break;
        }
    }
    return string.substr(i, j - i + 1);
}

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    size_t count = 1;

    size_t pos = text.find(delim);
    while (pos != std::string_view::npos) {
        ++count;
        pos = text.find(delim, pos + delim.size());
    }

    std::vector<std::string_view> ans;
    ans.reserve(count);
    pos = text.find(delim);
    while (pos != std::string_view::npos) {
        ans.emplace_back(text.substr(0, pos));
        text.remove_prefix(pos + delim.size());
        pos = text.find(delim);
    }
    ans.emplace_back(text);
    return ans;
}

std::string ReadN(const std::string& filename, size_t n) {
    int fd = open(filename.data(), O_RDONLY);
    if (fd == -1) {
        return "";
    }
    std::string buffer;
    buffer.resize(n);
    read(fd, buffer.data(), n);
    close(fd);
    return buffer;
}

std::string AddSlash(std::string_view path) {
    std::string s;
    s.reserve(path.size() + 1);
    for (char c : path) {
        s.push_back(c);
    }
    if (path.back() != '/') {
        s.push_back('/');
    }
    return s;
}

std::string_view RemoveSlash(std::string_view path) {
    if (!path.empty() && path != "/" && path[path.size() - 1] == '/') {
        path.remove_suffix(1);
    }
    return path;
}

std::string_view Dirname(std::string_view path) {
    auto j = path.find_last_of('/');
    if (j == 0) {
        return "/";
    }
    return path.substr(0, path.find_last_of('/'));
}

std::string_view Basename(std::string_view path) {
    return path.substr(path.find_last_of('/') + 1, std::string_view::npos);
}

std::string CollapseSlashes(std::string_view path) {
    std::string ans;
    ans.reserve(path.size());
    for (size_t i = 0; i < path.size(); ++i) {
        if (path[i] != '/') {
            ans += path[i];
        } else {
            ans += '/';
            while (path[i + 1] == '/') {
                ++i;
            }
        }
    }
    return ans;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return "";
    }
    size_t n = delimiter.size() * (strings.size() - 1);
    for (size_t i = 0; i < strings.size(); ++i) {
        n += strings[i].size();
    }
    std::string ans;
    ans.reserve(n);
    ans += strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        ans += delimiter;
        ans += strings[i];
    }
    return ans;
}

void Count(size_t& count, [[maybe_unused]] int digit) {
    count += 30;
}
void Count(size_t& count, [[maybe_unused]] long digit) {
    count += 30;
}
void Count(size_t& count, [[maybe_unused]] long long digit) {
    count += 30;
}
void Count(size_t& count, [[maybe_unused]] unsigned int digit) {
    count += 60;
}
void Count(size_t& count, [[maybe_unused]] unsigned long digit) {
    count += 60;
}
void Count(size_t& count, [[maybe_unused]] unsigned long long digit) {
    count += 60;
}
void Count(size_t& count, const std::string& digit) {
    count += digit.size();
}
void Count(size_t& count, const std::string_view digit) {
    count += digit.size();
}
void Count(size_t& count, const char* digit) {
    count += strlen(digit);
}
void Count(size_t& count, char* digit) {
    count += strlen(digit);
}

void RecConcat(std::string& s, int d) {
    if (d) {
        RecConcat(s, d / 10);
        s.push_back('0' + std::abs(d % 10));
    }
}
void Concat(std::string& s, int d) {
    if (d == 0) {
        s.push_back('0');
    } else {
        if (d < 0) {
            s.push_back('-');
        }
        RecConcat(s, d);
    }
}
void RecConcat(std::string& s, long long d) {
    if (d) {
        RecConcat(s, d / 10);
        s.push_back('0' + std::abs(d % 10));
    }
}
void Concat(std::string& s, long long d) {
    if (d == 0) {
        s.push_back('0');
    } else {
        if (d < 0) {
            s.push_back('-');
        }
        RecConcat(s, d);
    }
}
void RecConcat(std::string& s, long d) {
    if (d) {
        RecConcat(s, d / 10);
        s.push_back('0' + std::abs(d % 10));
    }
}
void Concat(std::string& s, long d) {
    if (d == 0) {
        s.push_back('0');
    } else {
        if (d < 0) {
            s.push_back('-');
        }
        RecConcat(s, d);
    }
}
void RecConcat(std::string& s, unsigned int d) {
    if (d) {
        RecConcat(s, d / 10);
        s.push_back('0' + d % 10);
    }
}
void Concat(std::string& s, unsigned int d) {
    if (d == 0) {
        s.push_back('0');
    } else {
        RecConcat(s, d);
    }
}
void RecConcat(std::string& s, unsigned long d) {
    if (d) {
        RecConcat(s, d / 10);
        s.push_back('0' + d % 10);
    }
}
void Concat(std::string& s, unsigned long d) {
    if (d == 0) {
        s.push_back('0');
    } else {
        RecConcat(s, d);
    }
}
void RecConcat(std::string& s, unsigned long long d) {
    if (d) {
        RecConcat(s, d / 10);
        s.push_back('0' + d % 10);
    }
}
void Concat(std::string& s, unsigned long long d) {
    if (d == 0) {
        s.push_back('0');
    } else {
        RecConcat(s, d);
    }
}

void Concat(std::string& s, const std::string& str) {
    for (size_t i = 0; i < str.size(); ++i) {
        s.push_back(str[i]);
    }
}
void Concat(std::string& s, const std::string_view str) {
    for (size_t i = 0; i < str.size(); ++i) {
        s.push_back(str[i]);
    }
}
void Concat(std::string& s, const char* str) {
    for (size_t i = 0; i < strlen(str); ++i) {
        s.push_back(str[i]);
    }
}
void Concat(std::string& s, char* str) {
    for (size_t i = 0; i < strlen(str); ++i) {
        s.push_back(str[i]);
    }
}
