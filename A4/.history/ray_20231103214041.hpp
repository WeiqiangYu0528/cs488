// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

// Represents a simple point light.
class Ray {
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction);
    Ray(const glm::vec3& origin, const glm::vec3& direction, const double t_lower_bound);
    glm::vec3 getPosition(double t) const;
    glm::vec3 origin;
    glm::vec3 direction;
    double t_lower_bound;
};

