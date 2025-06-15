#pragma once

#include <numeric>
#include <vector>

namespace mw::Aggregators {

template <class T>
const auto LastValue = [](const std::vector<T>& values) {
    return values.empty() ? T() : values.back();
};

template <class T>
const auto Sum = [](const std::vector<T>& values) {
    return std::accumulate(values.begin(), values.end(), T());
};

template <class T>
const auto Avg = [](const std::vector<T>& values) {
    if (values.empty()) {
        return T();
    }
    return std::accumulate(values.begin(), values.end(), T()) / values.size();
};

template <class T>
const auto Max = [](const std::vector<T>& values) {
    return values.empty() ? T() : *max_element(values.begin(), values.end());
};

template <class T>
const auto Min = [](const std::vector<T>& values) {
    return values.empty() ? T() : *min_element(values.begin(), values.end());
};

}