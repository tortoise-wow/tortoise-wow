#include "WeightedPermutation.h"
#include <array>
#include <chrono>
#include <iostream>
#include <numeric>
#include <stdexcept>

static void Require(bool ok, char const* reason)
{
    if (!ok) throw std::runtime_error(reason);
}

int main()
{
    std::mt19937 random(2147);
    for (size_t n : {0, 1, 2, 1000, 6000, 20000})
    {
        std::vector<unsigned> data(n), weights(n);
        std::iota(data.begin(), data.end(), 0);
        for (unsigned i = 0; i < n; ++i) weights[i] = i % 7;
        BotScheduling::WeightedPermutation(data.begin(), data.end(), weights.begin(), weights.end(), random);
        bool seenZero = false;
        for (unsigned i = 0; i < n; ++i)
        {
            Require(weights[i] == data[i] % 7, "item/weight pairing changed");
            if (!weights[i]) seenZero = true;
            else Require(!seenZero, "zero weight selected before positive weight");
        }
        std::sort(data.begin(), data.end());
        for (unsigned i = 0; i < n; ++i) Require(data[i] == i, "lost or duplicate candidate");
    }
    std::array<unsigned, 3> first{}, secondAfterZero{}, allZero{};
    constexpr unsigned Trials = 100000;
    for (unsigned trial = 0; trial < Trials; ++trial)
    {
        std::vector<unsigned> data{0,1,2}, weights{1,3,6};
        BotScheduling::WeightedPermutation(data.begin(), data.end(), weights.begin(), weights.end(), random);
        ++first[data[0]];
        if (data[0] == 0) ++secondAfterZero[data[1]];
        weights = {0,0,0};
        BotScheduling::WeightedPermutation(data.begin(), data.end(), weights.begin(), weights.end(), random);
        ++allZero[data[0]];
    }
    Require(std::abs(double(first[0])/Trials - .1) < .01, "weight 1 selection bias");
    Require(std::abs(double(first[1])/Trials - .3) < .01, "weight 3 selection bias");
    Require(std::abs(double(first[2])/Trials - .6) < .01, "weight 6 selection bias");
    Require(std::abs(double(secondAfterZero[1])/first[0] - 1.0/3) < .025,
        "conditional selection not proportional to remaining weights");
    for (auto count : allZero)
        Require(std::abs(double(count)/Trials - 1.0/3) < .01, "all-zero fallback not uniform");

    // Reference algorithm from all three ManTech playerbot trees. Timings are
    // informational, not flaky wall-clock pass/fail thresholds.
    constexpr size_t N = 12000;
    std::vector<unsigned> data(N), weights(N);
    std::iota(data.begin(), data.end(), 0);
    std::fill(weights.begin(), weights.end(), 10);
    auto start = std::chrono::steady_clock::now();
    for (size_t i=0; i<N; ++i)
    {
        std::discrete_distribution<size_t> choose(weights.begin()+i, weights.end());
        size_t j = i + choose(random);
        std::swap(data[i], data[j]);
        std::swap(weights[i], weights[j]);
    }
    auto oldUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
    start = std::chrono::steady_clock::now();
    BotScheduling::WeightedPermutation(data.begin(), data.end(), weights.begin(), weights.end(), random);
    auto newUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
    std::cout << "PASS weighted permutation; n=" << N << " reference_us=" << oldUs << " new_us=" << newUs << '\n';
}
