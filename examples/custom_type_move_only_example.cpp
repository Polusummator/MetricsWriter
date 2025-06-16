#include "MetricsWriter.h"

#include <iostream>

class MoveOnly {
public:
    MoveOnly() = default;
    explicit MoveOnly(int x) : x_(x) {}

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;

    bool operator<(const MoveOnly& other) const {
        return x_ < other.x_;
    }

    friend MoveOnly operator+(MoveOnly&& lhs, MoveOnly&& rhs) {
        return MoveOnly(lhs.x_ + rhs.x_);
    }

    template <typename T>
    MoveOnly operator/(T divisor) && {
        if (divisor == T{0}) {
            throw std::runtime_error("Division by zero");
        }
        return MoveOnly(static_cast<int>(x_ / divisor));
    }

    ~MoveOnly() = default;

    friend std::ostream& operator<<(std::ostream& out, const MoveOnly& t);
private:
    int x_;
};

std::ostream& operator<<(std::ostream& out, const MoveOnly& t) {
    out << t.x_ << "@";
    return out;
}

int main() {
    mw::MetricsWriter mw("metrics.log");

    auto& metric_last = mw.registerMetric<MoveOnly>("last");
    auto& metric_sum = mw.registerMetric<MoveOnly>("sum", mw::Aggregators::Sum<MoveOnly>);
    auto& metric_avg = mw.registerMetric<MoveOnly>("avg", mw::Aggregators::Avg<MoveOnly>);
    auto& metric_max = mw.registerMetric<MoveOnly>("max", mw::Aggregators::Max<MoveOnly>);
    auto& metric_min = mw.registerMetric<MoveOnly>("min", mw::Aggregators::Avg<MoveOnly>);

    std::thread t1([&] {
        for (int i = 0; i < 15; ++i) {
            metric_last.setValue(MoveOnly(i));
            metric_sum.setValue(MoveOnly(i));
            metric_avg.setValue(MoveOnly(i));
            metric_max.setValue(MoveOnly(i));
            metric_min.setValue(MoveOnly(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
    });
    std::thread t2([&] {
        for (int i = 0; i < 16; ++i) {
            metric_last.setValue(MoveOnly(i));
            metric_sum.setValue(MoveOnly(i));
            metric_avg.setValue(MoveOnly(i));
            metric_max.setValue(MoveOnly(i));
            metric_min.setValue(MoveOnly(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(350));
        }
    });
    t1.join();
    t2.join();

    std::cout << metric_last.getName() << " " << metric_last.getValue() << std::endl;
    std::cout << metric_sum.getName() << " " << metric_sum.getValue() << std::endl;
    std::cout << metric_avg.getName() << " " << metric_avg.getValue() << std::endl;
    std::cout << metric_max.getName() << " " << metric_max.getValue() << std::endl;
    std::cout << metric_min.getName() << " " << metric_min.getValue() << std::endl;

    mw.stop();
    return 0;
}