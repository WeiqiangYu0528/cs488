// Termm--Fall 2023

#include "Primitive.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

std::pair<glm::vec3, glm::vec3> NonhierSphere::getIntersection(Ray& ray) {
    double A = 
    double B = 
    double C = 
    size_t num = quadraticRoots(double A, double B, double C, double roots[2]);
}

NonhierBox::~NonhierBox()
{
}
