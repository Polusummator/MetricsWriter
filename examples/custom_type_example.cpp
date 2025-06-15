#include "MetricsWriter.h"

#include <iostream>
#include <utility>

class MyType {
public:
    MyType() : x_(-1), y_("hello") {}
    MyType(int x, std::string y) : x_(x), y_(std::move(y)) {}

    friend std::ostream& operator<<(std::ostream& out, const MyType& t);

private:
    int x_;
    std::string y_;
};

std::ostream& operator<<(std::ostream& out, const MyType& t) {
    out << t.x_ << "@" << t.y_;
    return out;
}

int main() {
    mw::MetricsWriter mw("metrics.log");

    auto& metric = mw.registerMetric<MyType>("something");

    for (int i = 0; i < 10; ++i) {
        metric.setValue(MyType(i, std::to_string(i + 10)));
        std::this_thread::sleep_for(std::chrono::milliseconds(490));
    }
    mw.stop();
    return 0;
}