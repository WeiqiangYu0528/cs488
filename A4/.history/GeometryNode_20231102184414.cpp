// Termm--Fall 2023

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

bool GeometryNode::intersect(Ray& ray, IntersectionData& data) {
	ray.origin = glm::vec3(invtrans * glm::vec4(ray.origin, 1.0f));
	ray.direction = glm::vec3(invtrans * glm::vec4(ray.direction, 0.0f));

	bool intersected = false;
	if (m_primitive->intersect(ray, data)) {
		intersected = true;
		
	}

	for (SceneNode * child : children) {
		if (child->intersect(ray, data)) {
			intersected = true;
		}
	}

	if (intersected) {
		data.position = glm::vec3(trans * glm::vec4(data.position, 1.0f));
		data.normal = glm::vec3(glm::transpose(glm::inverse(trans)) * glm::vec4(data.normal, 0.0f));
	}

	ray.origin = glm::vec3(trans * glm::vec4(ray.origin, 1.0f));
	ray.direction = glm::vec3(trans * glm::vec4(ray.direction, 0.0f));
	return intersected;
}