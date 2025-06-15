#include "MetricsWriter.h"

#include <random>

int main() {
    mw::MetricsWriter mw("metrics.log");

    auto& latency_max_metric = mw.registerMetric<double>("latency_max", mw::Aggregators::Max<double>);
    auto& latency_min_metric = mw.registerMetric<double>("latency_min", mw::Aggregators::Min<double>);
    auto& latency_avg_metric = mw.registerMetric<double>("latency_avg", mw::Aggregators::Avg<double>);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> latency_dist(0.0, 200.0);

    for (int i = 0; i < 10; ++i) {
        double latency = latency_dist(gen);
        latency_max_metric.setValue(latency);
        latency_min_metric.setValue(latency);
        latency_avg_metric.setValue(latency);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    mw.stop();
    return 0;
}