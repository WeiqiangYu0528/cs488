#include "ray.hpp"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {
    // empty
    t_lower_bound = 0.0;
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, const double t_lower_bound) :
    origin(origin), direction(direction), t_lower_bound(t_lower_bound) {
    // empty
}

glm::vec3 Ray::getPosition(double t) const {
    return origin + static_cast<float>(t) * direction;
}
