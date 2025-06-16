#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <utility>

#include <concurrentqueue.h>

namespace mw {

template <class Value, class Aggregator = std::function<Value(std::vector<Value>&&)>>
class Metric {
    friend class MetricsWriter;
public:
    explicit Metric(std::string name, Aggregator aggregator)
    : name_(std::move(name)), aggregator_(std::move(aggregator)) {}

    Value getValue() {
        std::lock_guard guard(mutex_);
        return std::move(current_value_);
    }

    std::string getName() const {
        return name_;
    }

    template <class ValueRef>
    void setValue(ValueRef&& value) {
        std::lock_guard guard(mutex_);
        current_value_ = std::forward<ValueRef>(value);
        queue_.enqueue(std::forward<ValueRef>(value));
    }

private:
    Value getAggregatedReset() {
        std::vector<Value> values;
        Value val;
        while (queue_.try_dequeue(val)) {
            values.push_back(std::move(val));
        }
        return aggregator_(std::move(values));
    }

private:
    std::string name_;
    Value current_value_;
    Aggregator aggregator_;

    mutable std::mutex mutex_;
    moodycamel::ConcurrentQueue<Value> queue_;
};

}
