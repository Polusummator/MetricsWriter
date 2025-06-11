#pragma once

#include <string>
#include <utility>

template <class Value>
class Metric {
public:
    explicit Metric(std::string name) : name_(std::move(name)) {}

    Value getValue() const {
        return value_;
    }

    std::string getName() const {
        return name_;
    }

    void setValue(Value value) {
        value_ = value;
    }

    void resetValue() {
        value_ = Value();
    }

private:
    std::string name_;
    Value value_;
};
