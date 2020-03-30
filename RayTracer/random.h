#ifndef RANDOMH
#define RANDOMH
#pragma once

#include <functional>
#include <random>

inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    static std::function<double()> rand_generator = std::bind(distribution, generator);
    return rand_generator();
}

inline double random_double(double min, double max)
{
    return min + random_double() * (max - min);
}

#endif