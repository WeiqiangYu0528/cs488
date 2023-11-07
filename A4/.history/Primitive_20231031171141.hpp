// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <ray.hpp>
#include <utility>

class Primitive {
public:
  virtual ~Primitive();
  virtual double getIntersection(Ray& ray) = 0;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  double getIntersection(Ray& ray);
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  double getIntersection(Ray& ray);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  double getIntersection(Ray& ray);
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  double getIntersection(Ray& ray);
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
