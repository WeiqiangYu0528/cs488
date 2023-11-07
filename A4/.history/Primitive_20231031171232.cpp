// Termm--Fall 2023

#include "Primitive.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

double Sphere::getIntersection(Ray& ray) {

}

Cube::~Cube()
{
}

double Cube::getIntersection(Ray& ray) {

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
    // if both roots are positive, select the closest one
    if (roots[0] > 0 && roots[1] > 0) {
        return std::min(roots[0], roots[1]);
    }
    // if one root is positive, return the maximum one
    if (roots[0] > 0 || roots[1] > 0) {
        return std::max(roots[0], roots[1]);
    }
    // otherwise, no intersection
    return -1;
}

NonhierBox::~NonhierBox()
{

}

double NonhierBox::getIntersection(Ray& ray) {
    
}
