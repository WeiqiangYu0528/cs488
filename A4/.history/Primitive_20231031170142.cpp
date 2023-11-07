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
    if (res < 0) {
        return -1;
    }
    if (res == 1 && (roots[0] > 0 || roots[1] > 0)) {
        return 
    }
    return std::min(roots);
}

NonhierBox::~NonhierBox()
{

}
