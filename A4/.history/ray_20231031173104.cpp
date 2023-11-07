#include "ray.hpp"

Ray::Ray(glm::vec3& origin, glm::vec3& direction) : origin(origin), direction(direction) {
    // empty
}

Ray::getPosition(double t) {
    return origin + t * direction * t;
}

