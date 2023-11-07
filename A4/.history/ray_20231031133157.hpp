// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

// Represents a simple point light.
class Ray {
  Ray(glm::vec3 origin, glm::vec3 direction);
  
  
  glm::vec3 origin;
  glm::vec3 direction;
};

