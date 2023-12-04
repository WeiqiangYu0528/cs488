// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include "Random.hpp"

class Perlin {
public:
    Perlin();
    ~Perlin();
    float noise(const glm::vec3& point) const;
    float turbulence(glm::vec3 point, const int depth) const;
    void perlin_generate_perm();
    void permute(int* perm);
    void setVertical(bool vertical);
    bool isVertical() const;
    glm::vec3 random_float();

private:
    const int point_cnt = 256;
    glm::vec3* random_floats;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    const float minValue = -0.9999999999f;
    const float maxValue = 0.9999999999f;

    bool vertical;
    
    Random randf;
};
