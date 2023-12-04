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
	std::string code, mtl_filename, mtl_name;
	double vx, vy, vz;
	double tu, tv;
	double nx, ny, nz;
	size_t s1, s2, s3, s4, s5, s6, s7, s8, s9;
	char discard;

	std::string filename = "Assets/" + fname;
	std::ifstream ifs( filename );
	while( ifs >> code ) {
		if (code == "mtllib") {
			ifs >> mtl_filename;
			readMaterial(mtl_filename);
		} else if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if ( code == "vt" ) {
			ifs >> tu >> tv;
			m_texture_coords.push_back( glm::vec2(tu, tv) );
		} else if ( code == "vn" ) {
			ifs >> nx >> ny >> nz;
			m_normals.push_back( glm::vec3(nx, ny, nz) );
		} else if( code == "f" ) {
			ifs >> s1 >> discard >> s2 >> discard >> s3 >> s4 >> discard >> s5 >> discard >> s6 >> s7 >> discard >> s8 >> discard >> s9;
			std::array<size_t, 3> v = { s1 - 1, s4 - 1, s7 - 1 };
			std::array<size_t, 3> t = { s2 - 1, s5 - 1, s8 - 1 };
			std::array<size_t, 3> n = { s3 - 1, s6 - 1, s9 - 1 };
			m_faces.push_back( Triangle( v, t, n, mtl_name ));
		} else if ( code == "usemtl") {
			ifs >> mtl_name;
		}
	}


	std::cout << "Loaded " << m_vertices.size() << " vertices." << std::endl;
	std::cout << "Loaded " << m_faces.size() << " faces." << std::endl;
	std::cout << "Loaded " << m_texture_coords.size() << " texture coordinates." << std::endl;
	std::cout << "Loaded " << m_normals.size() << " normals." << std::endl;

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

void Mesh::readMaterial(std::string& mtl_filename) {
	std::string mtl_file = "Assets/" + mtl_filename;
	std::ifstream ifs(mtl_file);
	std::string mtl_code, mtl_name, img_name;
	// glm::vec3 ka, ks;
	// double shininess, r, g, b;
	while (ifs >> mtl_code) {
		if (mtl_code == "newmtl") {
			ifs >> mtl_name;
		} 
		// else if(mtl_code == "Ns") {
		// 	ifs >> shininess;
		// } else if(mtl_code == "Ka") {
		// 	ifs >> r >> g >> b;
		// 	ka = glm::vec3(r, g, b);
		// } else if(mtl_code == "Ks") {
		// 	ifs >> r >> g >> b;
		// 	ks = glm::vec3(r, g, b);
		// }
		else if(mtl_code == "map_Kd") {
			ifs >> img_name;
			mat_img_map[mtl_name] = img_name;
			// m_materials[mtl_name] = std::make_shared<ImageMaterial>(ka, ks, shininess, img_name);
		}
	}
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
	glm::vec3 v0(m_vertices[triangle.vertices[0]]);
	glm::vec3 v1(m_vertices[triangle.vertices[1]]);
	glm::vec3 v2(m_vertices[triangle.vertices[2]]);

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
		float w = 1 - u - v;
		data.normal = glm::normalize(w * m_normals[triangle.normals[0]] + u * m_normals[triangle.normals[1]] + v * m_normals[triangle.normals[2]]);
		data.img_name = mat_img_map[triangle.mtl_name];
		data.uv = w * m_texture_coords[triangle.texcoords[0]] + u * m_texture_coords[triangle.texcoords[1]] + v * m_texture_coords[triangle.texcoords[2]];
		// uint x = uv.x * image.width();
		// uint y = (1 - uv.y) * image.height();
		// glm::vec3 diffuseColor(image(x, y, 0), image(x, y, 1), image(x, y, 2));
		return true;
	}
	
	return false;
}

bool Mesh::isMeshObject() const {
	return true;
}
