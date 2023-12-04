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

bool Sphere::isMeshObject() const {
    return false;
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

bool Cube::isMeshObject() const {
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
    bool intersected = t > ray.t_lower_bound && t < data.t;
    if (intersected) {
        data.t = t;
        data.position = ray.getPosition(t);
        data.normal = data.position - m_pos;
    }

    return intersected;
}

bool NonhierSphere::isMeshObject() const {
    return false;
}

NonhierBox::NonhierBox(const glm::vec3& pos, double size) :
    m_pos(pos), m_size(size)
{
    glm::vec3 max_pos = m_pos + glm::vec3(m_size, m_size, m_size);
    m_box = std::make_unique<NonhierBoxExtension>(m_pos, max_pos);
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::isMeshObject() const {
    return false;
}

bool NonhierBox::intersect(Ray& ray, IntersectionData& data) {
    return m_box->intersect(ray, data);
}

NonhierBoxExtension::NonhierBoxExtension(const glm::vec3& min_pos, const glm::vec3& max_pos) :
    min_pos(min_pos), max_pos(max_pos)
{

}
bool NonhierBoxExtension::intersect(Ray& ray, IntersectionData& data) {
    glm::vec3 invdir(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);
    glm::vec3 normal;

    double t_min, t_max, t_xmin, t_xmax, t_ymin, t_ymax, t_zmin, t_zmax;

    if (invdir.x >= 0) {
        t_xmin = (min_pos.x - ray.origin.x) * invdir.x;
        t_xmax = (max_pos.x - ray.origin.x) * invdir.x;
    } else {
        t_xmin = (max_pos.x - ray.origin.x) * invdir.x;
        t_xmax = (min_pos.x - ray.origin.x) * invdir.x;
    }

    if (invdir.y >= 0) {
        t_ymin = (min_pos.y - ray.origin.y) * invdir.y;
        t_ymax = (max_pos.y - ray.origin.y) * invdir.y;
    } else {
        t_ymin = (max_pos.y - ray.origin.y) * invdir.y;
        t_ymax = (min_pos.y - ray.origin.y) * invdir.y;
    }


    if ((t_xmin > t_ymax) || (t_ymin > t_xmax)) {
        return false;
    }

    t_min = std::max(t_xmin, t_ymin);
    t_max = std::min(t_xmax, t_ymax);

    if (invdir.z >= 0) {
        t_zmin = (min_pos.z - ray.origin.z) * invdir.z;
        t_zmax = (max_pos.z - ray.origin.z) * invdir.z;
    } else {
        t_zmin = (max_pos.z - ray.origin.z) * invdir.z;
        t_zmax = (min_pos.z - ray.origin.z) * invdir.z;
    }

    if ((t_min > t_zmax) || (t_zmin > t_max)){
        return false;
    }

    t_min = std::max(t_min, t_zmin);
    t_max = std::min(t_max, t_zmax);

    if (t_min < 0) {
        t_min = t_max;
        if (t_min < 0) {
            return false;
        }
    }

    if (t_min == t_xmin) {
        normal = glm::vec3(-1.0f, 0.0f, 0.0f) * invdir.x;
    } else if (t_min == t_ymin) {
        normal = glm::vec3(0.0f, -1.0f, 0.0f) * invdir.y;
    } else if (t_min == t_zmin) {
        normal =  glm::vec3(0.0f, 0.0f, -1.0f) * invdir.z;
    } else if (t_min == t_xmax) {
        normal = glm::vec3(1.0f, 0.0f, 0.0f) * invdir.x;
    } else if (t_min == t_ymax) {
        normal = glm::vec3(0.0f, 1.0f, 0.0f) * invdir.y;
    } else if (t_min == t_zmax) {
        normal = glm::vec3(0.0f, 0.0f, 1.0f) * invdir.z;
    }

    bool intersected = t_min > ray.t_lower_bound && t_min < data.t;
    if (intersected) {
        data.t = t_min;
        data.position = ray.getPosition(t_min);
        data.normal = glm::normalize(normal);
        // std::cout << data.normal.x << " " << data.normal.y << " " << data.normal.z << std::endl;
    }

    // if (!intersected && ray.type == RayType::Refraction) {
    //     std::cout << "error" << std::endl;
    // }
    return intersected;

}

NonhierBoxExtension::~NonhierBoxExtension() {
}

bool NonhierBoxExtension::isMeshObject() const {
    return false;
}

NonhierCylinder::NonhierCylinder(const glm::vec3& center, const double radius, const double height) :
    center(center), radius(radius), height(height)
{
}

NonhierCylinder::~NonhierCylinder() {
}

bool NonhierCylinder::isMeshObject() const {
    return false;
}

bool NonhierCylinder::intersect(Ray& ray, IntersectionData& data) {
    double t = -1.0, t_side = -1.0, t_cap = -1.0;
    double A = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
    double B = 2 * (ray.direction.x * (ray.origin.x - center.x) + ray.direction.z * (ray.origin.z - center.z));
    double C = (ray.origin.x - center.x) * (ray.origin.x - center.x) + (ray.origin.z - center.z) * (ray.origin.z - center.z) - radius * radius;
    double roots[2] = {0.0, 0.0};
    double t_caps[2] = {0.0, 0.0};
    size_t res = quadraticRoots(A, B, C, roots);
    double y_min = center.y - height / 2.0;
    double y_max = center.y + height / 2.0;
    glm::vec3 normal;

    // check whether the intersection point is valid (y_min <= y <= y_max)
    if (roots[0] > 0) {
        double y_intersected = ray.origin.y + static_cast<float>(roots[0]) * ray.direction.y;
        if (y_intersected < y_min || y_intersected > y_max) {
            roots[0] = -1.0;
        }
    }

    if (roots[1] > 0) {
        double y_intersected = ray.origin.y + static_cast<float>(roots[1]) * ray.direction.y;
        if (y_intersected < y_min || y_intersected > y_max) {
            roots[1] = -1.0;
        }
    }

    // if both roots are positive, select the closest one
    if (roots[0] > 0 && roots[1] > 0) {
        t_side = std::min(roots[0], roots[1]);
    }
    // if one root is positive, return the maximum one
    else if (roots[0] > 0 || roots[1] > 0) {
        t_side = std::max(roots[0], roots[1]);
    }

    t_caps[0] = (y_min - ray.origin.y) / ray.direction.y;
    t_caps[1] = (y_max - ray.origin.y) / ray.direction.y;

    // check whether the intersection point is in the circle (x^2 + z^2 <= r^2)
    if (t_caps[0] > 0) {
        glm::vec3 intersected_point = ray.getPosition(t_caps[0]);
        if (intersected_point.x * intersected_point.x + intersected_point.z * intersected_point.z > radius * radius) {
            t_caps[0] = -1.0;
        }
    }

    if (t_caps[1] > 0) {
        glm::vec3 intersected_point = ray.getPosition(t_caps[1]);
        if (intersected_point.x * intersected_point.x + intersected_point.z * intersected_point.z > radius * radius) {
            t_caps[1] = -1.0;
        }
    }

    // if both roots are positive, select the closest one
    if (t_caps[0] > 0 && t_caps[1] > 0) {
        t_cap = std::min(t_caps[0], t_caps[1]);
    }
    // if one root is positive, return the maximum one
    else if (t_caps[0] > 0 || t_caps[1] > 0) {
        t_cap = std::max(t_caps[0], t_caps[1]);
    }

    // if both roots are positive, select the closest one
    if (t_side > 0 && t_cap > 0) {
        t = std::min(t_side, t_cap);
    // if one root is positive, return the maximum one
    } else if (t_side > 0 || t_cap > 0) {
        t = std::max(t_side, t_cap);
    }
 
    bool intersected = t > ray.t_lower_bound && t < data.t;
    if (intersected) {
        data.t = t;
        data.position = ray.getPosition(t);
        if ( t == t_side ) {
            data.normal = glm::normalize(glm::vec3(data.position.x, center.y, data.position.z) - center);
        } else {
            data.normal = glm::normalize(glm::vec3(center.x, data.position.y, center.z) - center);
        }
    }

    return intersected;
}

Cylinder::Cylinder() {
    m_cylinder = std::make_unique<NonhierCylinder>(glm::vec3(0.0, 0.0, 0.0), 1.0, 2.0);
}

Cylinder::~Cylinder() {
}

bool Cylinder::isMeshObject() const {
    return false;
}

bool Cylinder::intersect(Ray& ray, IntersectionData& data) {
    return m_cylinder->intersect(ray, data);
}

Plane::Plane() {
    m_plane = std::make_unique<NonhierPlane>(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 1);
}

Plane::~Plane() {
}

bool Plane::isMeshObject() const {
    return false;
}

bool Plane::intersect(Ray& ray, IntersectionData& data) {
    return m_plane->intersect(ray, data);
}

NonhierPlane::NonhierPlane(const glm::vec3& pos, const glm::vec3& normal, const double size) :
    m_pos(pos), m_normal(normal), m_size(size)
{
}

NonhierPlane::~NonhierPlane() {
}

bool NonhierPlane::isMeshObject() const {
    return false;
}

bool NonhierPlane::intersect(Ray& ray, IntersectionData& data) {
    const float EPSILON = 1e-6;
    double len = glm::length(ray.direction);
    glm::vec3 l = glm::normalize(ray.direction);
    float denom = glm::dot(m_normal, l);
    bool intersected = false;
    if (std::abs(denom) > EPSILON) {
        double t = (glm::dot(m_pos - ray.origin, m_normal) / denom) / len;
        glm::vec3 intersected_point = ray.getPosition(t);

        intersected = (t > ray.t_lower_bound) &&
                      (t < data.t) &&
                      (intersected_point.x >= m_pos.x) &&
                      (intersected_point.x <= m_pos.x + m_size) &&
                      (intersected_point.z >= m_pos.z) &&
                      (intersected_point.z <= m_pos.z + m_size);

        if (intersected) {
            data.t = t;
            data.position = intersected_point;
            data.normal = m_normal;
            double image_size = m_size / 1;
            data.uv = glm::vec2((intersected_point.x - m_pos.x) / image_size, (intersected_point.z - m_pos.z) / image_size);
        }
    }
    return intersected;
}

NonhierCone::NonhierCone(const double height) :
    m_height(height)
{
}

NonhierCone::~NonhierCone() {
}

bool NonhierCone::isMeshObject() const {
    return false;
}

bool NonhierCone::intersect(Ray& ray, IntersectionData& data) {
    double t = -1.0, t_side = -1.0, t_cap = -1.0;
    double A = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z - ray.direction.y * ray.direction.y;
    double B = 2 * (ray.origin.x * ray.direction.x + ray.origin.z * ray.direction.z - ray.origin.y * ray.direction.y);
    double C = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - ray.origin.y * ray.origin.y;
    double roots[2] = {0.0, 0.0};
    size_t res = quadraticRoots(A, B, C, roots);
    double y_min = -m_height;
    double y_max = 0.0;
    glm::vec3 normal;

      // check whether the intersection point is valid (y_min <= y <= y_max)
    if (roots[0] > 0) {
        double y_intersected = ray.origin.y + static_cast<float>(roots[0]) * ray.direction.y;
        if (y_intersected < y_min || y_intersected > y_max) {
            roots[0] = -1.0;
        }
    }

    if (roots[1] > 0) {
        double y_intersected = ray.origin.y + static_cast<float>(roots[1]) * ray.direction.y;
        if (y_intersected < y_min || y_intersected > y_max) {
            roots[1] = -1.0;
        }
    }

    // if both roots are positive, select the closest one
    if (roots[0] > 0 && roots[1] > 0) {
        t_side = std::min(roots[0], roots[1]);
    }
    // if one root is positive, return the maximum one
    else if (roots[0] > 0 || roots[1] > 0) {
        t_side = std::max(roots[0], roots[1]);
    }

    t_cap = (y_min - ray.origin.y) / ray.direction.y;

    // check whether the intersection point is in the circle (x^2 + z^2 <= r^2)
    if (t_cap > 0) {
        glm::vec3 intersected_point = ray.getPosition(t_cap);
        if (intersected_point.x * intersected_point.x + intersected_point.z * intersected_point.z > m_height * m_height) {
            t_cap = -1.0;
        }
    }

    // if both roots are positive, select the closest one
    if (t_side > 0 && t_cap > 0) {
        t = std::min(t_side, t_cap);
    // if one root is positive, return the maximum one
    } else if (t_side > 0 || t_cap > 0) {
        t = std::max(t_side, t_cap);
    }
 
    bool intersected = t > ray.t_lower_bound && t < data.t;
    if (intersected) {
        data.t = t;
        data.position = ray.getPosition(t);
        if ( t == t_side ) {
            float y_normal = glm::sqrt(data.position.x * data.position.x + data.position.z * data.position.z);
            data.normal = glm::normalize(glm::vec3(data.position.x, y_normal, data.position.z));
        } else {
            data.normal = glm::vec3(0, -1, 0);
        }
    }

    return intersected;
}

Cone::Cone() {
    m_cone = std::make_unique<NonhierCone>(1.0);
}

Cone::~Cone() {
}

bool Cone::isMeshObject() const {
    return false;
}

bool Cone::intersect(Ray& ray, IntersectionData& data) {
    return m_cone->intersect(ray, data);
}