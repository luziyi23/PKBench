#ifndef __GAUSS_GENERATOR_HPP__
#define __GAUSS_GENERATOR_HPP__

#include <stdlib.h>
#include <math.h>

#include <random>
#include <cstdint>


class GaussIntGenerator
{
public:
    GaussIntGenerator(uint64_t mean, uint64_t std) : rand_(), generator_(rand_()), dist_(mean, std) { Next(); }

    uint64_t Next();
    uint64_t Last();

private:
    std::random_device rand_;
    std::mt19937_64 generator_;
    std::normal_distribution<double> dist_;
    uint64_t last_int_;
};

inline uint64_t GaussIntGenerator::Next()
{
    return last_int_ = round(dist_(generator_));
}

inline uint64_t GaussIntGenerator::Last()
{
    return last_int_;
}
#endif