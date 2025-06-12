#pragma once

#include <mutex>
#include <string>
#include <utility>

template <class Value>
class Metric {
public:
    explicit Metric(std::string name) : name_(std::move(name)) {}

    Value getValue() const {
        std::lock_guard guard(mutex_);
        return value_;
    }

    std::string getName() const {
        return name_;
    }

    void setValue(Value value) {
        std::lock_guard guard(mutex_);
        value_ = value;
    }

    void resetValue() {
        std::lock_guard guard(mutex_);
        value_ = Value();
    }

    Value getAndResetValue() {
        std::lock_guard guard(mutex_);
        auto result = value_;
        value_ = Value();
        return result;
    }

private:
    std::string name_;
    Value value_;
    std::mutex mutex_;
};
