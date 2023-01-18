#pragma once
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

bool StartsWith(std::string_view string, std::string_view text);
bool EndsWith(std::string_view string, std::string_view text);

std::string_view StripSuffix(std::string_view string, std::string_view prefix);
std::string_view StripPrefix(std::string_view string, std::string_view prefix);

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n = std::string::npos);

std::string_view StripAsciiWhitespace(std::string_view string);

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);

std::string ReadN(const std::string& filename, size_t n);

std::string AddSlash(std::string_view path);
std::string_view RemoveSlash(std::string_view path);

std::string_view Dirname(std::string_view path);
std::string_view Basename(std::string_view path);

std::string CollapseSlashes(std::string_view path);
std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);

void Count(size_t& count, [[maybe_unused]] int digit);
void Count(size_t& count, [[maybe_unused]] long digit);
void Count(size_t& count, [[maybe_unused]] long long digit);
void Count(size_t& count, [[maybe_unused]] unsigned int digit);
void Count(size_t& count, [[maybe_unused]] unsigned long digit);
void Count(size_t& count, [[maybe_unused]] unsigned long long digit);
void Count(size_t& count, const std::string& digit);
void Count(size_t& count, const std::string_view digit);
void Count(size_t& count, const char* digit);
void Concat(std::string& s, char* str);

template <typename T, typename... Args>
void Count(size_t& count, const T& digit, const Args&... digits) {
    Count(count, digit);
    Count(count, digits...);
}

void RecConcat(std::string& s, int d);
void RecConcat(std::string& s, long d);
void RecConcat(std::string& s, long long d);
void RecConcat(std::string& s, unsigned int d);
void RecConcat(std::string& s, unsigned long d);
void RecConcat(std::string& s, unsigned long long d);
void Concat(std::string& s, int d);
void Concat(std::string& s, long d);
void Concat(std::string& s, long long d);
void Concat(std::string& s, unsigned int d);
void Concat(std::string& s, unsigned long d);
void Concat(std::string& s, unsigned long long d);
void Concat(std::string& s, const std::string& str);
void Concat(std::string& s, const std::string_view str);
void Concat(std::string& s, const char* str);
void Concat(std::string& s, char* str);

template <typename T, typename... Args>
void Concat(std::string& s, const T& digit, const Args&... digits) {
    Concat(s, digit);
    Concat(s, digits...);
}

template <typename... Args>
std::string StrCat(const Args&... digits) {
    if constexpr (sizeof...(digits) == 0) {
        return {};
    } else {
        size_t count = 0;
        Count(count, digits...);
        std::string s;
        s.reserve(count);
        Concat(s, digits...);
        return s;
    }
}
