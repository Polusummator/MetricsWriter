#include "MetricsWriter.h"

#include <thread>
#include <chrono>
#include <random>

int main() {
    mw::MetricsWriter mw("metrics.log");

    auto& diff_metric = mw.registerMetric<int>("diff",
        [](const std::vector<int>& values) {
            int cnt0 = 0;
            int cnt1 = 0;
            for (int i : values) {
                if (i % 2 == 0) {
                    cnt0++;
                }
                else {
                    cnt1++;
                }
            }
            return cnt0 - cnt1;
        });

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> numbers_dist(0, 100);

    for (int i = 0; i < 50; ++i) {
        diff_metric.setValue(numbers_dist(gen));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    mw.stop();
    return 0;
}