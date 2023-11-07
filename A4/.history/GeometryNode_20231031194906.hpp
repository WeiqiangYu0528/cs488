// Termm--Fall 2023

#pragma once

#include "IntersectionData.hpp"
#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"


#include <glm/glm.hpp>
#include <utility>

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	bool intersect(Ray& ray, IntersectionData& data);
	double getIntersection(Ray& ray);

	Material *m_material;
	Primitive *m_primitive;
};
