// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <utility>

#include "IntersectionData.hpp"
#include "ray.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool isMeshObject() const = 0;
  virtual bool intersect(Ray& ray, IntersectionData& data) = 0;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  bool isMeshObject() const;
  bool intersect(Ray& ray, IntersectionData& data);
  std::unique_ptr<Primitive> m_sphere;
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  bool isMeshObject() const;
  bool intersect(Ray& ray, IntersectionData& data);
  std::unique_ptr<Primitive> m_cube;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  bool intersect(Ray& ray, IntersectionData& data);
  bool isMeshObject() const;
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  bool intersect(Ray& ray, IntersectionData& data);
  bool isMeshObject() const;
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
  std::unique_ptr<Primitive> m_box;
};

class NonhierBoxExtension : public Primitive {
public:
  NonhierBoxExtension(const glm::vec3& min_pos, const glm::vec3& max_pos);
  bool intersect(Ray& ray, IntersectionData& data);
  bool isMeshObject() const;
  virtual ~NonhierBoxExtension();

private:
  glm::vec3 min_pos, max_pos;
};

class NonhierCylinder: public Primitive {
  public:
    NonhierCylinder(const glm::vec3& center, const double radius, const double height);
    bool intersect(Ray& ray, IntersectionData& data);
    bool isMeshObject() const;
    virtual ~NonhierCylinder();

  private:
    glm::vec3 center;
    double radius;
    double height;
};

class Cylinder: public Primitive {
  public:
    Cylinder();
    bool intersect(Ray& ray, IntersectionData& data);
    bool isMeshObject() const;
    virtual ~Cylinder();

  private:
    std::unique_ptr<Primitive> m_cylinder;
};

class NonhierPlane: public Primitive {
  public:
    NonhierPlane(const glm::vec3& pos, const glm::vec3& normal, const double size);
    bool intersect(Ray& ray, IntersectionData& data);
    bool isMeshObject() const;
    virtual ~NonhierPlane();

  private:
    glm::vec3 m_pos;
    glm::vec3 m_normal;
    double m_size;
};

class Plane: public Primitive {
  public:
    Plane();
    bool intersect(Ray& ray, IntersectionData& data);
    bool isMeshObject() const;
    virtual ~Plane();

  private:
    std::unique_ptr<Primitive> m_plane;
};

class Cone: public Primitive {
  public:
    Cone();
    bool intersect(Ray& ray, IntersectionData& data);
    bool isMeshObject() const;
    virtual ~Cone();

  private:
    std::unique_ptr<Primitive> m_cone;
};

class NonhierCone: public Primitive {
  public:
    NonhierCone(const double height);
    bool intersect(Ray& ray, IntersectionData& data);
    bool isMeshObject() const;
    virtual ~NonhierCone();

  private:
    double m_height;
};