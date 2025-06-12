#pragma once

#include <Metric.h>

#include <memory>
#include <unordered_map>
#include <sstream>

class MetricsWriter {
public:
    explicit MetricsWriter(std::string filename) : filename_(std::move(filename)) {}

    MetricsWriter(const MetricsWriter&) = delete;
    MetricsWriter& operator=(const MetricsWriter&) = delete;
    MetricsWriter(MetricsWriter&&) = delete;
    MetricsWriter& operator=(MetricsWriter&&) = delete;

    ~MetricsWriter() {
        stop();
    }

    template <class T>
    Metric<T>& registerMetric(const std::string& name) {
        auto wrapper = std::make_unique<MetricWrapperImpl<T>>(name);
        Metric<T>& metric = wrapper->metric_;
        metrics_[name] = std::move(wrapper);
        return metric;
    }

    void stop() {
        // todo
    }

private:
    struct MetricWrapper {
        virtual std::string getName() = 0;
        virtual std::string getStringAndReset() = 0;
    };

    template <class T>
    struct MetricWrapperImpl : MetricWrapper {
        Metric<T> metric_;

        explicit MetricWrapperImpl(const std::string& name) : metric_(name) {}

        std::string getName() override {
            return metric_.getName();
        }

        std::string getStringAndReset() override {
            std::ostringstream ss;
            ss << metric_.getValue();
            metric_.resetValue();
            return ss.str();
        }
    };

    std::string filename_;
    std::unordered_map<std::string, std::unique_ptr<MetricWrapper>> metrics_;

    void dumpValues() {
        // todo
    }
};