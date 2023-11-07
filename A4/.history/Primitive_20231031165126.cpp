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

std::pair<glm::vec3, glm::vec3> NonhierSphere::getIntersection(Ray& ray) {
    glm::vec3 distance = ray.origin - m_pos;
    double A = glm::dot(ray.direction, ray.direction);
    double B = 2 * glm::dot(ray.direction, distance);
    double C = glm::dot(distance, distance) - m_radius * m_radius;
    double roots[2];
    size_t res = quadraticRoots(A, B, C, roots);
    if 
}

NonhierBox::~NonhierBox()
{

}
