#pragma once

#include <glm/glm.hpp>
#include <limits>
#include "Material.hpp"

struct IntersectionData {
    double t = ;
    glm::vec3 position;
    glm::vec3 normal;
    Material* material;
};
