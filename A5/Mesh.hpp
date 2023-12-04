// Termm--Fall 2023

#pragma once

#include <array>
#include <memory>
#include <vector>
#include <iosfwd>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Image.hpp"
#include "ImageMaterial.hpp"
#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES

struct Triangle
{
	std::array<size_t, 3> vertices;
	std::array<size_t, 3> texcoords;
	std::array<size_t, 3> normals;
	std::string mtl_name;

	Triangle( std::array<size_t, 3>& v, std::array<size_t, 3>& t, std::array<size_t, 3>& n, std::string mtl_name )
		: vertices( v ),
		  texcoords( t ),
		  normals( n ),
		  mtl_name( mtl_name )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  Mesh(std::vector<glm::vec3> vertices, std::vector<Triangle> faces);
  bool isMeshObject() const;
  bool intersect(Ray& ray, IntersectionData& data);
  bool intersectTriangle(Ray& ray, IntersectionData& data, Triangle& triangle);
  void readMaterial(std::string& mtl_filename);
  std::unordered_map<std::string, std::string> mat_img_map;
  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_texture_coords;
	std::vector<glm::vec3> m_normals;
	std::vector<Triangle> m_faces;
	// std::unordered_map<std::string, std::shared_ptr<ImageMaterial>> m_materials;
	std::unique_ptr<NonhierBoxExtension> m_bounding_box;
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
