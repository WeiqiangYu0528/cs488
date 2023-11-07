// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

// Represents a simple point light.
struct Ray {
  Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {});
  
  glm::vec3 origin;
  glm::vec3 direction;
};

