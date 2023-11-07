#include "ray.hpp"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {
    // empty
}

Ray(const glm::vec3& origin, const glm::vec3& direction, const double t_lower_bound);

glm::vec3 Ray::getPosition(double t) const {
    return origin + static_cast<float>(t) * direction;
}

