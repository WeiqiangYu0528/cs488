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
    // std::vector<glm::vec3> vertices = {
    //     glm::vec3(m_pos.x, m_pos.y, m_pos.z),
    //     glm::vec3(m_pos.x + m_size, m_pos.y, m_pos.z),
    //     glm::vec3(m_pos.x, m_pos.y + m_size, m_pos.z),
    //     glm::vec3(m_pos.x + m_size, m_pos.y + m_size, m_pos.z),
    //     glm::vec3(m_pos.x, m_pos.y, m_pos.z + m_size),
    //     glm::vec3(m_pos.x + m_size, m_pos.y, m_pos.z + m_size),
    //     glm::vec3(m_pos.x, m_pos.y + m_size, m_pos.z + m_size),
    //     glm::vec3(m_pos.x + m_size, m_pos.y + m_size, m_pos.z + m_size)
    // };

    // std::vector<Triangle> faces = {
    //     Triangle(0, 1, 2),
    //     Triangle(1, 3, 2),
    //     Triangle(0, 1, 4),
    //     Triangle(1, 5, 4),
    //     Triangle(2, 3, 6),
    //     Triangle(3, 7, 6),
    //     Triangle(0, 6, 2),
    //     Triangle(0, 4, 6),
    //     Triangle(1, 5, 3),
    //     Triangle(3, 5, 7),
    //     Triangle(4, 7, 6),
    //     Triangle(4, 5, 7)
    // };

    // m_mesh = std::make_unique<Mesh>(vertices, faces);
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(Ray& ray, IntersectionData& data) {
    std::cout<< "here" << std::endl;
    const float Epsilon = 0.000001f;
    glm::vec3 normal, normalx, normaly, normalz;

    double t_min, t_max, t_xmin, t_xmax, t_ymin, t_ymax, t_zmin, t_zmax;
    if (fabs(ray.direction.x < Epsilon)) {
        if (ray.origin.x < m_pos.x || ray.origin.x > m_pos.x + m_size) {
            return false;
        }
    } else {
        if (ray.direction.x >= 0) {
            t_xmin = (m_pos.x - ray.origin.x) / ray.direction.x;
            t_xmax = (m_pos.x + m_size - ray.origin.x) / ray.direction.x;
            normalx = glm::vec3(-1.0f, 0.0f, 0.0f);
        } else {
            t_xmin = (m_pos.x + m_size - ray.origin.x) / ray.direction.x;
            t_xmax = (m_pos.x - ray.origin.x) / ray.direction.x;
            normalx = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }

    if (fabs(ray.direction.y < Epsilon)) {
        if (ray.origin.y < m_pos.y || ray.origin.y > m_pos.y + m_size) {
            return false;
        }
    } else {
        if (ray.direction.y >= 0) {
            t_ymin = (m_pos.y - ray.origin.y) / ray.direction.y;
            t_ymax = (m_pos.y + m_size - ray.origin.y) / ray.direction.y;
            normaly = glm::vec3(0.0f, -1.0f, 0.0f);
        } else {
            t_ymin = (m_pos.y + m_size - ray.origin.y) / ray.direction.y;
            t_ymax = (m_pos.y - ray.origin.y) / ray.direction.y;
            normaly = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }

    if ((tmin > tymax) || (tymin > tmax))
    return false;

    if (fabs(ray.direction.z < Epsilon)) {
        if (ray.origin.z < m_pos.z || ray.origin.z > m_pos.z + m_size) {
            return false;
        }
    } else {
        if (ray.direction.z >= 0) {
            t_zmin = (m_pos.z - ray.origin.z) / ray.direction.z;
            t_zmax = (m_pos.z + m_size - ray.origin.z) / ray.direction.z;
            normalz = glm::vec3(0.0f, 0.0f, -1.0f);
        } else {
            t_zmin = (m_pos.z + m_size - ray.origin.z) / ray.direction.z;
            t_zmax = (m_pos.z - ray.origin.z) / ray.direction.z;
            normalz = glm::vec3(0.0f, 0.0f, 1.0f);
        }
    }

    if (t_xmin > t_ymin && t_xmin > t_zmin) {
        t_min = t_xmin;
        normal = normalx;
    }
    else if (t_ymin > t_xmin && t_ymin > t_zmin) {
        t_min = t_ymin;
        normal = normaly;
    }
    else {
        t_min = t_zmin;
        normal = normalz;
    }
    t_max = std::min(t_zmax, std::min(t_xmax, t_ymax));

    if (intersected) {
        data.t = t_min;
        data.position = ray.getPosition(t_min);
        data.normal = normal;
    }

    return intersected;
}
