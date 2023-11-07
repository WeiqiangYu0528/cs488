#include "ray.hpp"

Ray::Ray(glm::vec3& origin, glm::vec3& direction) : origin(origin), direction(direction) {
    // empty
}

glm::vec3 Ray::getPosition(double t) {
    return origin + static_cast<float>(t) * direction;
}

