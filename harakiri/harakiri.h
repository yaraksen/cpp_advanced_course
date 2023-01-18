#pragma once

#include <string>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
public:
    virtual ~OneTimeCallback() {
    }
    virtual std::string operator()() const&& = 0;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback(std::string s) {
        this->s_ = new std::string(s);
    }
    ~AwesomeCallback() {
        delete this->s_;
    }

    std::string operator()() const&& {
        std::string new_s = *this->s_ + "awesomeness";
        delete this;
        return new_s;
    }

private:
    std::string* s_;
};
