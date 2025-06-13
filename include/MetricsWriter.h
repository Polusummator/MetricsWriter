#pragma once

#include "Metric.h"
#include "utils/timestamp.h"

#include <memory>
#include <map>
#include <sstream>
#include <fstream>
#include <thread>
#include <atomic>
#include <condition_variable>

class MetricsWriter {
public:
    template <typename Rep, typename Period>
    MetricsWriter(std::string filename, std::chrono::duration<Rep, Period> period)
    : filename_(std::move(filename)), period_(period), running_(true) {
        writingThread_ = std::thread(&MetricsWriter::runWriting, this);
    }

    explicit MetricsWriter(std::string filename)
    : filename_(std::move(filename)), period_(1), running_(true) {
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
            cv_running_.notify_one();
            writingThread_.join();
        }
    }

private:
    struct MetricWrapper {
        virtual std::string getName() const = 0;
        virtual std::string getStringAndReset() = 0;
    };

    template <class T>
    struct MetricWrapperImpl : MetricWrapper {
        Metric<T> metric_;

        explicit MetricWrapperImpl(const std::string& name) : metric_(name) {}

        std::string getName() const override {
            return metric_.getName();
        }

        std::string getStringAndReset() override {
            std::ostringstream ss;
            ss << metric_.getAggregatedReset();
            return ss.str();
        }
    };

private:
    void runWriting() {
        std::unique_lock lock(mutex_running_);
        auto next_wakeup = std::chrono::steady_clock::now() + period_;
        while (running_) {
            while (std::chrono::steady_clock::now() < next_wakeup && running_) {
                cv_running_.wait_until(lock, next_wakeup);
            }
            if (!running_) {
                return;
            }

            dumpValues();
            next_wakeup += period_;
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
    const std::chrono::seconds period_;
    std::map<std::string, std::unique_ptr<MetricWrapper>> metrics_;

    std::condition_variable cv_running_;
    mutable std::mutex mutex_running_;
    std::atomic<bool> running_;

    std::thread writingThread_;
    mutable std::mutex mutex_;
};
