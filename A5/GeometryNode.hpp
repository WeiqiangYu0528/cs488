// Termm--Fall 2023

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <utility>

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	bool intersect(Ray& ray, IntersectionData& data);

	Material *m_material;
	Primitive *m_primitive;
};
