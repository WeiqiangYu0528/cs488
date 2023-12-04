// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

// Represents a simple point light.
struct AreaLight {
  AreaLight(glm::vec3& position, glm::vec3& u_vec, glm::vec3& v_vec, glm::vec3& w_vec, size_t u_steps, size_t v_steps, size_t w_steps, glm::vec3& colour, glm::vec3& falloff);
  
  glm::vec3 position;
  glm::vec3 u_vec;
  glm::vec3 v_vec;
  glm::vec3 w_vec;
  size_t u_steps;
  size_t v_steps;
  size_t w_steps;
  glm::vec3 colour;
  glm::vec3 falloff;
  
  float getArea() const;
};

