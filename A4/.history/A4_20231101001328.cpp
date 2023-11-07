// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A4.hpp"
#include "IntersectionData.hpp"
#include "ray.hpp"

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...  

  std::cout << "F23: Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			glm::vec3 direction = getDirection(eye, view, up, fovy, w, h, x, y);
			Ray ray(eye, direction);
			IntersectionData data;
			glm::vec3 color;
			if (root->intersect(ray, data)) {
				PhonngMaterial* material = dynamic_cast<PhonngMaterial*>(data.material);
				color += material->getDiffuse() * ambient;
			}
			// Red: 
			image(x, y, 0) = (double)color.x;
			// Green: 
			image(x, y, 1) = (double)color.y;
			// Blue: 
			image(x, y, 2) = (double)color.z;
		}
	}
}

glm::vec3 getDirection(const glm::vec3& eye, const glm::vec3& view, const glm::vec3& up, double fovy,
					   size_t width, size_t height, float x, float y) {
	float wf = static_cast<float>(width);
	float hf = static_cast<float>(height);

	glm::vec3 w = glm::normalize(view - eye);
	glm::vec3 u = glm::normalize(glm::cross(up, w));
	glm::vec3 v = glm::normalize(glm::cross(w, u));
	glm::vec3 w_p = (-wf / 2.0 ) * u + (hf / 2.0) * v - ((hf / 2.0) / tanf(glm::radians(fovy) * 0.5)) * w;

	glm::vec3 direction = glm::normalize(x * u + y * (-v) + w_p);
	return direction;
}
