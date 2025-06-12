#include "MetricsWriter.h"

#include <thread>
#include <chrono>
#include <random>

int main() {
    MetricsWriter mw("metrics.log");

    auto& cpu_metric = mw.registerMetric<double>("CPU");
    auto& http_metric = mw.registerMetric<int>("HTTP requests RPS");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> cpu_dist(0.0, 4.0);
    std::uniform_int_distribution<int> http_dist(0, 1000);

    for (int i = 0; i < 60; ++i) {
        cpu_metric.setValue(cpu_dist(gen));
        http_metric.setValue(http_dist(gen));

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}