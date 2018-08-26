#pragma once

#include <algorithm>
#include <vector>

#include "Math\CrusMath.h"

namespace isle::math
{
template<typename T>
T constexpr gaussianDistribution(T x, T sigma, T EV = 0)
{
    return std::exp(static_cast<T>(-0.5) * std::pow((x - EV) / sigma, 2)) / (sigma * std::sqrt(static_cast<T>(kPI * 2)));
}

template<typename T>
constexpr i32 PixelsNeededForSigma(T sigma, T threshold)
{
    auto const size = 1 + 2 * sigma * std::sqrt(-2 * std::log(threshold * static_cast<T>(0.01)));

    return static_cast<i32>(std::floor(size)) | 1;
}

template<typename T>
constexpr T GetSigmaBasedOnTapSize(i32 size, T threshold)
{
    return static_cast<T>(size - 1) / (2 * std::sqrt(-2 * std::log(threshold * static_cast<T>(0.01))));
}

template<typename T>
constexpr T GaussianSimpsonIntegration(T a, T b, T sigma)
{
    return ((b - a) / static_cast<T>(6)) * (gaussianDistribution(a, sigma) + 4 * gaussianDistribution((a + b) / static_cast<T>(2), sigma) + gaussianDistribution(b, sigma));
}

template<typename T>
constexpr std::vector<T> GaussianKernelIntegrals(T sigma, i32 taps, bool half_size = true, bool normalize = true)
{
    auto const half_taps = taps >> 1;

    std::vector<T> weights(taps);

    std::generate(weights.begin(), weights.end(), [sigma, half_taps, i = 0]() mutable
    {
        auto const x = static_cast<T>(i++ - half_taps);
        return GaussianSimpsonIntegration(x - static_cast<T>(0.5), x + static_cast<T>(0.5), sigma);
    });

    if (normalize) {
        auto total = std::accumulate(weights.begin(), weights.end(), static_cast<T>(0));

        std::transform(weights.begin(), weights.end(), weights.begin(), [total] (auto value) {
            return value / total;
        });
    }

    if (half_size)
        weights.erase(weights.begin(), std::next(weights.begin(), half_taps));

    return weights;
}
}