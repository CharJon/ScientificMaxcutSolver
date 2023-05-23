#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

#include "benchmark/benchmark.h"


static void cacheBench(benchmark::State &s) {
    int bytes = 1 << s.range(0);
    int count = (bytes / sizeof(int)) / 2;
    bool random = s.range(1);

    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()};  // Generates random integers

    std::uniform_int_distribution<int> dist{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
    auto gen = [&dist, &mersenne_engine]() {
        return dist(mersenne_engine);
    };

    std::vector<int> v(count);
    std::generate(begin(v), end(v), gen);


    std::vector<int> indices(count);
    if (random) {
        std::uniform_int_distribution<int> dist_indices{0, count};
        auto gen_indices = [&dist_indices, &mersenne_engine]() {
            return dist_indices(mersenne_engine);
        };
        std::generate(begin(indices), end(indices), gen_indices);
    } else {
        for (int i = 0; i < count; i++) {
            indices[i] = i;
        }
    }

    for (auto _ : s) {
        uint64_t sum = 0;
        for (int i: indices) sum += v[i];
        benchmark::DoNotOptimize(sum);
    }

    s.SetBytesProcessed(long(s.iterations()) * long(bytes));
    s.SetLabel(std::to_string(bytes / 1024) + "kB");
}

// Register the function as a benchmark
BENCHMARK(cacheBench)->ArgsProduct(
        {benchmark::CreateDenseRange(13, 25, 1),
         {0, 1}});
// Run the benchmark
BENCHMARK_MAIN();
