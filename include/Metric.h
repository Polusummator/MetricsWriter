#pragma once

#include <mutex>
#include <string>
#include <utility>

template <class Value>
class Metric {
    friend class MetricsWriter;
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
        aggregated_value_ = value_; // todo: custom aggregate
    }

    void resetValue() {
        std::lock_guard guard(mutex_);
        value_ = Value();
        aggregated_value_ = Value(); // todo: custom aggregate
    }

private:
    Value getAggregatedReset() {
        std::lock_guard guard(mutex_);
        auto result = aggregated_value_;
        aggregated_value_ = Value();
        return result;
    }

private:
    std::string name_;
    Value value_;
    Value aggregated_value_;
    mutable std::mutex mutex_;
};
