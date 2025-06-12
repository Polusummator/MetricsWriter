#pragma once

#include <Metric.h>
#include "utils/timestamp.h"

#include <memory>
#include <map>
#include <sstream>
#include <fstream>
#include <thread>
#include <atomic>

class MetricsWriter {
public:
    explicit MetricsWriter(std::string filename) : filename_(std::move(filename)), running_(true) {
        writingThread_ = std::thread(&MetricsWriter::runWriting, this);
    }

    MetricsWriter(const MetricsWriter&) = delete;
    MetricsWriter& operator=(const MetricsWriter&) = delete;
    MetricsWriter(MetricsWriter&&) = delete;
    MetricsWriter& operator=(MetricsWriter&&) = delete;

    ~MetricsWriter() {
        stop();
    }

    template <class T>
    Metric<T>& registerMetric(const std::string& name) {
        if (metrics_.find(name) != metrics_.end()) {
            throw std::runtime_error("Metric \"" + name + "\" already registered");
        }

        std::lock_guard guard(mutex_);
        auto wrapper = std::make_unique<MetricWrapperImpl<T>>(name);
        Metric<T>& metric = wrapper->metric_;
        metrics_[name] = std::move(wrapper);
        return metric;
    }

    void stop() {
        if (running_) {
            running_ = false;
            if (writingThread_.joinable()) {
                writingThread_.join();
            }
        }
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
            ss << metric_.getAndResetValue();
            return ss.str();
        }
    };

private:
    void runWriting() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            dumpValues();
        }
    }

    void dumpValues() {
        std::ofstream file(filename_, std::ios::out | std::ios::app);

        file << getTimestamp();

        std::lock_guard guard(mutex_);
        for (auto& metric : metrics_) {
            auto& wrapper = metric.second;
            file << " \"" << wrapper->getName() << "\" " << wrapper->getStringAndReset();
        }
        file << std::endl;
    }

private:
    const std::string filename_;
    std::map<std::string, std::unique_ptr<MetricWrapper>> metrics_;

    std::atomic<bool> running_;
    std::thread writingThread_;
    std::mutex mutex_;
};
