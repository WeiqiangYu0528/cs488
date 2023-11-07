// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A4.hpp"
#include "IntersectionData.hpp"
#include "PhongMaterial.hpp"
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
			getDirection(eye, view, up, fovy, w, h, x, h-y, direction);
			break;
			Ray ray(eye, direction);
			IntersectionData data;
			glm::vec3 color;
			if (root->intersect(ray, data)) {
				PhongMaterial* material = dynamic_cast<PhongMaterial*>(data.material);
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

// glm::vec3 getDirection(const glm::vec3& eye, const glm::vec3& view, const glm::vec3& up, double fovy,
// 					   size_t width, size_t height, float x, float y) {
// 	float wf = static_cast<float>(width);
// 	float hf = static_cast<float>(height);

// 	glm::vec3 w = glm::normalize(view - eye);
// 	glm::vec3 u = glm::normalize(glm::cross(up, w));
// 	glm::vec3 v = glm::normalize(glm::cross(w, u));

// 	glm::mat3 mat (u, v, w);
// 	glm::mat3 mat_inv = glm::transpose(mat);
// 	for (int i = 0; i < 3; ++i) {
// 		std::cout << mat_inv[i].x << " " << mat_inv[i].y << " " << mat_inv[i].z << std::endl;
// 	}

// 	glm::vec3 w_p = (-wf / 2.0 ) * mat[0] + (hf / 2.0) * mat[1] - ((hf / 2.0) / tanf(glm::radians(fovy) * 0.5)) * mat[2];

// 	glm::vec3 direction = glm::normalize(x * mat[0] + y * (-mat[1]) + w_p);
// 	std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
// 	return direction;
// }

glm::vec3 getDirection(const glm::vec3& eye, const glm::vec3& view, const glm::vec3& up, double fovy,
					   size_t width, size_t height, float x, float y) {
	float wf = static_cast<float>(width);
	float hf = static_cast<float>(height);

	glm::vec3 w = glm::normalize(view - eye);
	glm::vec3 u = glm::normalize(glm::cross(up, w));
	glm::vec3 v = glm::normalize(glm::cross(w, u));

	glm::mat4 c2w(u, v, w, glm::vec4(eye, 1.0f));
	float Px = (2 * ((x + 0.5) / imageWidth) - 1) * tanf(glm::radians(fov / 2));
	float Py = (1 - 2 * ((y + 0.5) / imageHeight)) * tanf(glm::radians(fov / 2));
	// glm::mat3 mat_inv = glm::transpose(mat);
	// for (int i = 0; i < 3; ++i) {
	// 	std::cout << mat_inv[i].x << " " << mat_inv[i].y << " " << mat_inv[i].z << std::endl;
	// }

	// glm::vec3 w_p = (-wf / 2.0 ) * mat[0] + (hf / 2.0) * mat[1] - ((hf / 2.0) / tanf(glm::radians(fovy) * 0.5)) * mat[2];
	glm::vec3
	glm::vec3 direction = glm::normalize(x * mat[0] + y * (-mat[1]) + w_p);
	std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
	return direction;
}

void getDirection(
	glm::vec3 eye, glm::vec3 view, glm::vec3 up,
	double fovy, size_t w, size_t h, float x, float y,
	glm::vec3& direction) {
		float wf = static_cast<float>(w), hf = static_cast<float>(h);
		float d = hf / (2 * glm::tan(glm::radians(fovy) / 2.0f));
		// Step 1
		glm::mat4 T1 = glm::translate(glm::mat4(), glm::vec3(-wf/2.0f, -hf/2.0f, d));
		glm::mat4 S2 = glm::scale(glm::mat4(), glm::vec3(-1.0f, 1.0f, 1.0f));

		glm::vec3 w_ = glm::normalize(view - eye);
		glm::vec3 u_ = glm::normalize(glm::cross(up, w_));
		glm::vec3 v_ = glm::normalize(glm::cross(w_, u_));
		glm::mat4 R3 = glm::transpose ( glm::mat4(
			u_.x, v_.x, w_.x, 0.0f,
			u_.y, v_.y, w_.y, 0.0f,
			u_.z, v_.z, w_.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		glm::mat4 T4 = glm::translate(glm::mat4(), glm::vec3(eye.x, eye.y, eye.z));
		glm::vec4 pixelCoor(x, y, 0.0f, 1.0f);
		glm::vec4 worldCoor = T4 * R3 * S2 * T1 * pixelCoor;
		direction = glm::normalize((glm::vec3(worldCoor) - eye));

		std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
}
