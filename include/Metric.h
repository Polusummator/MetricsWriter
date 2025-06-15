#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <utility>

#include <concurrentqueue.h>

namespace mw {

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

    void setValue(const Value& value) {
        std::lock_guard guard(mutex_);
        current_value_ = value;
        queue_.enqueue(value);
    }

private:
    Value getAggregatedReset() {
        std::vector<Value> values;
        Value val;
        while (queue_.try_dequeue(val)) {
            values.push_back(val);
        }
        return aggregator_(values);
    }

private:
    std::string name_;
    Value current_value_;
    Aggregator aggregator_;

    mutable std::mutex mutex_;
    moodycamel::ConcurrentQueue<Value> queue_;
};

}
