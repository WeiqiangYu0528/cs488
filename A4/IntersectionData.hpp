#pragma once

#include <glm/glm.hpp>
#include <limits>
#include "Material.hpp"

struct IntersectionData {
    double t = std::numeric_limits<double>::max();
    glm::vec3 position;
    glm::vec3 normal;
    Material* material;
};
