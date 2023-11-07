// Termm--Fall 2023
#include <algorithm>
#include "Primitive.hpp"
#include "polyroots.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(Ray& ray, IntersectionData& data) {
    return false;
}

Cube::~Cube()
{
}

bool Cube::intersect(Ray& ray, IntersectionData& data) {
    return false;
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::intersect(Ray& ray, IntersectionData& data) {
    glm::vec3 distance = ray.origin - m_pos;
    double t = -1.0;
    double A = glm::dot(ray.direction, ray.direction);
    double B = 2 * glm::dot(ray.direction, distance);
    double C = glm::dot(distance, distance) - m_radius * m_radius;
    double roots[2] = {0.0, 0.0};;
    size_t res = quadraticRoots(A, B, C, roots);
    // if both roots are positive, select the closest one
    if (roots[0] > 0 && roots[1] > 0) {
        t = std::min(roots[0], roots[1]);
    }
    // if one root is positive, return the maximum one
    else if (roots[0] > 0 || roots[1] > 0) {
        t = std::max(roots[0], roots[1]);
    }
    // otherwise, no intersection
    bool intersected = t != -1 && t < data.t;
    if (intersected) {
        data.t = t;
        data.position = ray.getPosition(t);
        data.normal = data.position - m_pos;
    }

    return intersected;
}

NonhierBox::~NonhierBox()
{
    glm::vec3[8] vertices = {
        glm::vec3(m_pos.x, m_pos.y, m_pos.z),
        gl
    }

}

bool NonhierBox::intersect(Ray& ray, IntersectionData& data) {

}
