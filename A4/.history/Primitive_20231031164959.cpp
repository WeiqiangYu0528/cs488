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
    glm::vec4 distance = 
    double A = glm::dot(ray.direction, ray.direction);
    double B = 2 * glm::dot(ray.direction, (ray.origin - m_pos));
    double C = glm::dot((ray.origin - m_pos), (ray.origin - m_pos)) - m_radius * m_radius;
    double roots[2];
    size_t res = quadraticRoots(A, B, C, roots);
}

NonhierBox::~NonhierBox()
{

}
