// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

// Represents a simple point light.
class Ray {
public:
    Ray(glm::vec3 origin, glm::vec3 direction);
    
private:
  glm::vec3 origin;
  glm::vec3 direction;
};

