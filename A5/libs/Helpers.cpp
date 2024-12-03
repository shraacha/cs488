#include "Helpers.hpp"

#include <cstdlib>
#include <random>

double getRand0To1()
{
    return (double)intRand(0, RAND_MAX) / (double)RAND_MAX;
}

double getRandNeg1To1()
{
    return getRand0To1() * 2 - 1;
}

int intRand(const int & min, const int & max)
{
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}
