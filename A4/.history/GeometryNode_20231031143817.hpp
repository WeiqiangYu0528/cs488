// Termm--Fall 2023

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	std::pair<glm::vec3, glm::vec3> getIntersection(Ray& ray);

	Material *m_material;
	Primitive *m_primitive;
};
