// Termm--Fall 2023

#include <iostream>
#include <fstream>
#include <limits>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "IntersectionData.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::string filename = "Assets/" + fname;
	std::ifstream ifs( filename );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}


	std::cout << "Loaded " << m_vertices.size() << " vertices." << std::endl;
	std::cout << "Loaded " << m_faces.size() << " faces." << std::endl;

	float x_min = std::numeric_limits<float>::max();
	float y_min = std::numeric_limits<float>::max();
	float z_min = std::numeric_limits<float>::max();
	float x_max = std::numeric_limits<float>::min();
	float y_max = std::numeric_limits<float>::min();
	float z_max	= std::numeric_limits<float>::min();

	for (auto& vertice: m_vertices) {
		x_min = std::min(x_min, vertice.x);
		y_min = std::min(y_min, vertice.y);
		z_min = std::min(z_min, vertice.z);
		x_max = std::max(x_max, vertice.x);
		y_max = std::max(y_max, vertice.y);
		z_max = std::max(z_max, vertice.z);
	}

	glm::vec3 box_min(x_min, y_min, z_min);
	glm::vec3 box_max(x_max, y_max, z_max);
	m_bounding_box = std::make_unique<NonhierBoxExtension>(box_min, box_max);
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<Triangle> faces) :
	m_vertices(vertices),
	m_faces(faces)
{
	// empty
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

bool Mesh::intersect(Ray& ray, IntersectionData& data) {
	bool intersected = false;
	// std::cout << m_vertices.size() << " " << m_faces.size() << std::endl;
	#ifdef RENDER_BOUNDING_VOLUMES
	intersected = m_bounding_box->intersect(ray, data);
	#else
	IntersectionData temp_data;
	if (m_bounding_box->intersect(ray, temp_data)) {
		for (Triangle& triangle : m_faces) {
			if (intersectTriangle(ray, data, triangle)) {
				intersected = true;
			}
		}
	}
	#endif
	// std::cout << data.t << " " << intersected << std::endl;
	return intersected;
}

bool Mesh::intersectTriangle(Ray& ray, IntersectionData& data, Triangle& triangle) {
	const float kEpsilon = 0.0000001f;
	glm::vec3 v0(m_vertices[triangle.v1]);
	glm::vec3 v1(m_vertices[triangle.v2]);
	glm::vec3 v2(m_vertices[triangle.v3]);

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 pvec = glm::cross(ray.direction, edge2);
    float det = glm::dot(pvec, edge1);

	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < kEpsilon) {
		return false;
	}
	float invDet = 1 / det;

	glm::vec3 tvec = ray.origin - v0;
	float u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) {
		return false;
	}

	glm::vec3 qvec = glm::cross(tvec, edge1);
	float v = glm::dot(ray.direction, qvec) * invDet;
	if (v < 0 || u + v > 1) {
		return false;
	}
	
	float t = glm::dot(edge2, qvec) * invDet;

	if (t > kEpsilon && t > ray.t_lower_bound && t < data.t) { // ray intersection
		data.t = t;
		data.position = ray.getPosition(t);
		data.normal = glm::cross(edge1, edge2);
		return true;
	}
	
	return false;
}
