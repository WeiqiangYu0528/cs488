// Termm--Fall 2023
#include <algorithm>
#include <iostream>
#include "Primitive.hpp"
#include "polyroots.hpp"
#include "Mesh.hpp"

Primitive::~Primitive()
{
}

Sphere::Sphere()
{
    m_sphere = std::make_unique<NonhierSphere>(glm::vec3(0.0, 0.0, 0.0), 1.0);
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(Ray& ray, IntersectionData& data) {
    return m_sphere->intersect(ray, data);
}

Cube::Cube() {
    m_cube = std::make_unique<NonhierBox>(glm::vec3(0.0, 0.0, 0.0), 1.0);
}

Cube::~Cube()
{
}

bool Cube::intersect(Ray& ray, IntersectionData& data) {
    return m_cube->intersect(ray, data);
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

NonhierBox::NonhierBox(const glm::vec3& pos, double size) :
    m_pos(pos), m_size(size)
{
    std::vector<glm::vec3> vertices = {
        glm::vec3(m_pos.x, m_pos.y, m_pos.z),
        glm::vec3(m_pos.x + m_size, m_pos.y, m_pos.z),
        glm::vec3(m_pos.x, m_pos.y + m_size, m_pos.z),
        glm::vec3(m_pos.x + m_size, m_pos.y + m_size, m_pos.z),
        glm::vec3(m_pos.x, m_pos.y, m_pos.z + m_size),
        glm::vec3(m_pos.x + m_size, m_pos.y, m_pos.z + m_size),
        glm::vec3(m_pos.x, m_pos.y + m_size, m_pos.z + m_size),
        glm::vec3(m_pos.x + m_size, m_pos.y + m_size, m_pos.z + m_size)
    };

    std::vector<Triangle> faces = {
        Triangle(0, 1, 2),
        Triangle(1, 3, 2),
        Triangle(0, 1, 4),
        Triangle(1, 5, 4),
        Triangle(2, 3, 6),
        Triangle(3, 7, 6),
        Triangle(0, 6, 2),
        Triangle(0, 4, 6),
        Triangle(1, 5, 3),
        Triangle(3, 5, 7),
        Triangle(4, 7, 6),
        Triangle(4, 5, 7)
    };

    m_mesh = std::make_unique<Mesh>(vertices, faces);
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(Ray& ray, IntersectionData& data) {
    double t_en
}
