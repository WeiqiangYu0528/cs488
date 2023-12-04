#include "ray.hpp"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, const double t_lower_bound, RayType type) :
    origin(origin), direction(direction), t_lower_bound(t_lower_bound), type(type) {
    // empty
}

glm::vec3 Ray::getPosition(double t) const {
    return origin + static_cast<float>(t) * direction;
}

