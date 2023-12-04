// Termm--Fall 2023

#include <glm/ext.hpp>

#include "AreaLight.hpp"

AreaLight::AreaLight(glm::vec3& position, glm::vec3& u_vec, glm::vec3& v_vec, glm::vec3& w_vec, size_t u_steps, size_t v_steps, size_t w_steps, glm::vec3& colour, glm::vec3& falloff)
    : position(position), u_vec(u_vec), v_vec(v_vec), w_vec(w_vec), u_steps(u_steps), v_steps(v_steps), w_steps(w_steps), colour(colour), falloff(falloff)
{
}

float AreaLight::getArea() const
{
    return glm::length(u_vec * u_steps) * glm::length(v_vec * v_steps) * glm::length(w_vec * w_steps);
}
