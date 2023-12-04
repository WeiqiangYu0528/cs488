#include "Random.hpp"
#include <iostream>

Random::Random(const float minValue, const float maxValue)
    : rd(), gen(rd()), dis(minValue, maxValue)
{
    // empty
}

float Random::randomFloat() {
    return dis(gen);
}
