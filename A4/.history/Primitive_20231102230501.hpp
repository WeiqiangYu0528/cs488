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
  virtual bool intersect(Ray& ray, IntersectionData& data) = 0;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  bool intersect(Ray& ray, IntersectionData& data);
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  bool intersect(Ray& ray, IntersectionData& data);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  bool intersect(Ray& ray, IntersectionData& data);
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  bool intersect(Ray& ray, IntersectionData& data);
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
  std::unique_ptr<Primitive> m_mesh;
};