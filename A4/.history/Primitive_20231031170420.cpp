// Termm--Fall 2023

#include "Primitive.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

double NonhierSphere::getIntersection(Ray& ray) {
    glm::vec3 distance = ray.origin - m_pos;
    double A = glm::dot(ray.direction, ray.direction);
    double B = 2 * glm::dot(ray.direction, distance);
    double C = glm::dot(distance, distance) - m_radius * m_radius;
    double roots[2] = {0.0, 0.0};;
    size_t res = quadraticRoots(A, B, C, roots);
    if (roots[0] > 0 && roots[1] > 0) {
        return std::min(roots[0], roots[1]);
    }
    if ((roots[0] > 0 || roots[1] > 0)) {
        return std::max(roots[0], roots[1]);
    }
    if (res == 2 && )
       
    return -1;
    return std::min(roots);
}

NonhierBox::~NonhierBox()
{

}
