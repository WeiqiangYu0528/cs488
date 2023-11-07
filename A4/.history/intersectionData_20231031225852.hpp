#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

struct IntersectionData {
    double t;
    double 
    glm::vec3 position;
    glm::vec3 normal;
    Material* material;
};
