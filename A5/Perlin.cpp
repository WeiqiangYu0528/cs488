// Termm--Fall 2023

#include "Perlin.hpp"
#include <iostream>
#include <ctime>

Perlin::Perlin() : randf(minValue, maxValue)
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    random_floats = new glm::vec3[point_cnt];
    for (size_t i = 0; i < point_cnt; i++) {
        random_floats[i] = random_float();
    }
    perlin_generate_perm();
    vertical = true;
}

Perlin::~Perlin()
{
    delete[] random_floats;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
}

void Perlin::perlin_generate_perm() {
    perm_x = new int[point_cnt];
    perm_y = new int[point_cnt];
    perm_z = new int[point_cnt];

    for (size_t i = 0; i < point_cnt; ++i) {
        perm_x[i] = i;
        perm_y[i] = i;
        perm_z[i] = i;
    }
    
    permute(perm_x);
    permute(perm_y);
    permute(perm_z);
}

void Perlin::permute(int* perm) {
    for (int i = point_cnt - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        std::swap(perm[i], perm[j]);
    }
}

float Perlin::noise(const glm::vec3& point) const {
    int x_ = std::floor(point.x);
    int y_ = std::floor(point.y);
    int z_ = std::floor(point.z);
    float u = point.x - x_;
    float v = point.y - y_;
    float w = point.z - z_;
    float u_ = u * u * (3 - 2 * u);
    float v_ = v * v * (3 - 2 * v);
    float w_ = w * w * (3 - 2 * w);

    float sum_ = 0.0;
    float val_ = 0.0;
    glm::vec3 weight_(0.0f);
    for (size_t i=0; i < 2; ++i) {
        for (size_t j=0; j < 2; ++j) {
            for (size_t k=0; k < 2; ++k) {
                weight_ = glm::vec3(u - i, v - j, w - k);
                val_ = glm::dot(random_floats[perm_x[(x_ + i) & 255] ^ perm_y[(y_ + j) & 255] ^ perm_z[(z_ + k) & 255]], weight_);
                sum_ += (i * u_ + (1 - i) * (1 - u_)) * (j * v_ + (1 - j) * (1 - v_)) * (k * w_ + (1 - k) * (1 - w_)) * val_;
            }
        }
    }
    return sum_;
}

float  Perlin::turbulence(glm::vec3 point, const int depth) const {
    float sum_ = 0.0;
    float weight_ = 1.0;

    for (size_t i = 0; i < depth; ++i) {
        sum_ += weight_ * noise(point);
        weight_ *= 0.5;
        point *= 2;
    }

    return std::abs(sum_);
}

glm::vec3 Perlin::random_float() {
    return glm::vec3(randf.randomFloat(), randf.randomFloat(), randf.randomFloat());
}

void Perlin::setVertical(bool vertical) {
    this->vertical = vertical;
}

bool Perlin::isVertical() const {
    return vertical;
}