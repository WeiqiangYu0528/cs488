// Termm--Fall 2023

#pragma once

#include <random>

class Random {
public:
    Random(const float minValue, const float maxValue);
    float randomFloat();

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
};


//   const float minValue = 0.0f;
//     const float maxValue = 0.9999999999f;