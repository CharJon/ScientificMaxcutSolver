#ifndef SMS_STATISTICS_HPP
#define SMS_STATISTICS_HPP

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <unordered_map>

template<typename T>
struct statistics
{
    T min;
    T max;
    double mean;
    T median;
    double standard_dev;
    T mode;
};

template<typename T>
T getMode(std::vector<T> values)
{
    assert(not values.empty());

    bool is_int = true;
    double intpart = 0;
    for (T x: values)
    {
        if (std::modf(x, &intpart) != 0.0)
            is_int = false;
    }

    if (is_int)
    {
        std::unordered_map<int, int> mode_map;
        for (int x: values)
        {
            mode_map[x] = 0;
        }

        int mode = 0;
        int mode_count = 0;

        for (int x: values)
        {
            mode_map[x] += 1;

            if (mode_map[x] > mode_count)
            {
                mode = x;
                mode_count = mode_map[x];
            }
        }

        return mode;
    } else
    {
        return -1;
    }
}

template<typename T>
statistics<T> getStatistics(std::vector<T> values)
{
    assert(not values.empty());

    uint n = values.size();
    T min = *std::min_element(values.begin(), values.end());
    T max = *std::max_element(values.begin(), values.end());

    auto mean = ((double) std::accumulate(values.begin(), values.end(), 0) / n);

    auto m = values.begin() + (n / 2) - 1;
    std::nth_element(values.begin(), m, values.end());
    T median = values[(n - 1) / 2];

    double standard_dev = 0;
    for (T x: values)
    {
        standard_dev += std::pow((x - mean), 2);
    }

    standard_dev /= n;
    standard_dev = std::sqrt(standard_dev);

    T mode = getMode(values);

    return {min, max, mean, median, standard_dev, mode};
}


#endif //SMS_STATISTICS_HPP