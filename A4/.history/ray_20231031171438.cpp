#include "ray.hpp"

Ray::Ray(const glm::vec3& origin, glm::vec3& direction) : origin(origin), direction(direction) {
    // empty
}

Ray::getPosition(double t) const {
    return origin + direction * t;
}

