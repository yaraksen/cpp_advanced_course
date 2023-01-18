#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <string>
#include <memory>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

class FullMatch {
public:
    FullMatch(std::string&& str) : str_(std::move(str)) {
    }
    bool operator()(std::string str) const {
        return str == str_;
    }

private:
    std::string str_;
};

class Substr {
public:
    Substr(std::string&& str) : str_(std::move(str)) {
    }
    bool operator()(std::string str) const {
        return str.find(str_) != std::string::npos;
    }

private:
    std::string str_;
};

class TestRegistry {
public:
    static TestRegistry& Instance() {
        return instance;
    }

    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        test_names_[class_name] = []() -> AbstractTest* { return new TestClass(); };
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        if (test_names_.count(class_name)) {
            return std::unique_ptr<AbstractTest>(test_names_[class_name]());
        } else {
            throw std::out_of_range("Test is not registered");
        }
    }

    void RunTest(const std::string& test_name) {
        auto test = CreateTest(test_name);
        test->SetUp();
        try {
            test->Run();
        } catch (...) {
            test->TearDown();
            throw;
        }
        test->TearDown();
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> ans;
        for (auto& pair : test_names_) {
            if (callback(pair.first)) {
                ans.push_back(pair.first);
            }
        }
        return ans;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> ans;
        for (auto& pair : test_names_) {
            ans.push_back(pair.first);
        }
        return ans;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (auto& pair : test_names_) {
            if (callback(pair.first)) {
                RunTest(pair.first);
            }
        }
    }

    void Clear() {
        test_names_.clear();
        // tests_.clear();
    }

private:
    static TestRegistry instance;
    std::map<std::string, std::function<AbstractTest*()>> test_names_;
    // std::unordered_map<std::string, std::unique_ptr<AbstractTest>> tests_;
};

TestRegistry TestRegistry::instance;
