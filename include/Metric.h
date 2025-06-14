#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <utility>

template <class Value, class Aggregator = std::function<Value(const std::vector<Value>&)>>
class Metric {
    friend class MetricsWriter;
public:
    explicit Metric(std::string name, Aggregator aggregator)
    : name_(std::move(name)), aggregator_(std::move(aggregator)) {}

    Value getValue() const {
        std::lock_guard guard(mutex_);
        return current_value_;
    }

    std::string getName() const {
        return name_;
    }

    void setValue(Value value) {
        std::lock_guard guard(mutex_);
        current_value_ = value;
        values_.push_back(value);
    }

private:
    Value getAggregatedReset() {
        std::lock_guard guard(mutex_);
        auto result = aggregator_(values_);
        values_.clear();
        return result;
    }

private:
    std::string name_;
    std::vector<Value> values_;
    Value current_value_;
    Aggregator aggregator_;
    mutable std::mutex mutex_;
};
