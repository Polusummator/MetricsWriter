#pragma once

#include <numeric>
#include <vector>

namespace mw::Aggregators {

template <class T>
const auto LastValue = [](std::vector<T>&& values) {
    return values.empty() ? T{} : std::move(values.back());
};

template <class T>
const auto Sum = [](std::vector<T>&& values) {
    T result{};
    for (auto& v : values) {
        result = std::move(result) + std::move(v);
    }
    return result;
};

template <class T>
const auto Avg = [](std::vector<T>&& values) {
    if (values.empty()) {
        return T{};
    }
    T result{};
    for (auto& v : values) {
        result = std::move(result) + std::move(v);
    }
    return std::move(result) / values.size();
};

template <class T>
const auto Max = [](std::vector<T>&& values) {
    return values.empty() ? T{} : std::move(*max_element(values.begin(), values.end()));
};

template <class T>
const auto Min = [](std::vector<T>&& values) {
    return values.empty() ? T{} : std::move(*min_element(values.begin(), values.end()));
};

}