# MetricsWriter

Header-only library for storing metrics in text format

```shell
2025-06-15 16:27:02.506 "CPU" 0.0862826 "HTTP requests RPS" 635
2025-06-15 16:27:03.506 "CPU" 2.70077 "HTTP requests RPS" 671
2025-06-15 16:27:04.506 "CPU" 1.03089 "HTTP requests RPS" 445
2025-06-15 16:27:05.505 "CPU" 1.53331 "HTTP requests RPS" 801
2025-06-15 16:27:06.506 "CPU" 3.05988 "HTTP requests RPS" 735
2025-06-15 16:27:07.505 "CPU" 1.66373 "HTTP requests RPS" 959
```

## Usage

### Basic example

```c++
#include "MetricsWriter.h"

#include <thread>
#include <chrono>
#include <random>

int main() {
    mw::MetricsWriter mw("metrics.log");

    auto& cpu_metric = mw.registerMetric<double>("CPU");
    auto& http_metric = mw.registerMetric<int>("HTTP requests RPS");

    for (int i = 0; i < 100; ++i) {
        cpu_metric.setValue(/*value*/);
        http_metric.setValue(/*value*/);
    }
    
    // you can get current value with getValue()
    int rps = http_metric.getValue();

    mw.stop(); // Not necessary, destructor calls stop
    return 0;
}
```
see full [example](examples/cpu_http_example.cpp)

### Period
You can also specify the period for writing to the file

```c++
mw::MetricsWriter mw("metrics.log", std::chrono::seconds(5));
```

Default value is `1s`. The passed number must be an integer number of seconds (you can also use minutes, hours etc.)

### Custom types

MetricsWriter works with metrics of any types that have:
- move support
- `operator<<`

For example, you can use your own type

```c++
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
    metric.setValue(MyType(1, std::to_string(10)));
    
    /*...*/
}
```
see full [example](examples/custom_type_example.cpp)

see move-only type [example](examples/custom_type_move_only_example.cpp)

### Aggregators

MetricsWriter dumps metrics' values every `period` seconds and resets aggregated values. The default aggregation function is simply taking the last value, but you can change this behaviour

Aggregator is a function `Value (std::vector<Value>&& values);`, that can be passed in `registerMetric`

There are some built-in aggregators:
- `LastValue` (default)
- `Sum`
- `Avg`
- `Max`
- `Min`

Note that they do not work for all types, because they require the implementation of certain operators 

```c++
auto& latency_max_metric = mw.registerMetric<double>("latency_max", mw::Aggregators::Max<double>);
auto& latency_min_metric = mw.registerMetric<double>("latency_min", mw::Aggregators::Min<double>);
auto& latency_avg_metric = mw.registerMetric<double>("latency_avg", mw::Aggregators::Avg<double>);
```
see full [example](examples/latency_example.cpp)

Or you can use your own aggregator

```c++
auto& diff_metric = mw.registerMetric<int>("diff",
        [](std::vector<int>&& values) {
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
```
see full [example](examples/custom_aggregator_example.cpp)

## Add to your project

The easiest way to use library is to add it using FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    mw
    GIT_REPOSITORY https://github.com/Polusummator/MetricsWriter.git
    GIT_TAG main
)
FetchContent_MakeAvailable(mw)

target_link_libraries(your_target_name metrics-writer)
```

## Build examples

```shell
cmake .
make cpu_http_example
```



