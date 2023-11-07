// Termm--Fall 2023

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
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

}

bool Mesh::intersectTriangle(Ray& ray, IntersectionData& data, Triangle& triangle) {
	const float kEpsilon = 0.0000001f;
	glm::vec3 v0(m_vertices[triangle.v1]);
	glm::vec3 v1(m_vertices[triangle.v2]);
	glm::vec3 v2(m_vertices[triangle.v3]);

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 pvec = glm::cross(v0v2);
    float det = v0v1.dotProduct(pvec);
	#ifdef CULLING
		// if the determinant is negative, the triangle is 'back facing'
		// if the determinant is close to 0, the ray misses the triangle
		if (det < kEpsilon) return false;
	#else
		// ray and triangle are parallel if det is close to 0
		if (fabs(det) < kEpsilon) return false;
	#endif
		float invDet = 1 / det;

		Vec3f tvec = orig - v0;
		u = tvec.dotProduct(pvec) * invDet;
		if (u < 0 || u > 1) return false;

		Vec3f qvec = tvec.crossProduct(v0v1);
		v = dir.dotProduct(qvec) * invDet;
		if (v < 0 || u + v > 1) return false;
		
		t = v0v2.dotProduct(qvec) * invDet;
		
		return true;
	#else
		...
	#endif
}