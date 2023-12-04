#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <memory>
#include <string>
#include "Material.hpp"

struct IntersectionData {
    double t = std::numeric_limits<double>::max();
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 colour = glm::vec3(0.0f);
    glm::vec3 initPos = glm::vec3(0.0f);
    glm::vec2 uv = glm::vec2(0.0f);
    std::string img_name = "";
    Material* material = nullptr;
};
