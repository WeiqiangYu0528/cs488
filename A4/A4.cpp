// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A4.hpp"
#include "IntersectionData.hpp"
#include "PhongMaterial.hpp"
#include "ray.hpp"

const double EPSILON1 = 1e-1;
const double EPSILON2 = 0;
const size_t numSamples = 4;
std::random_device rd;
std::mt19937 gen(rd());
const float minValue = 0.0f;
const float maxValue = 0.9999999999f;
std::uniform_real_distribution<float> dis(minValue, maxValue);

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

	Image backgroundImage;
	// backgroundImage.loadPng("sky.png");
	backgroundImage.loadPng("starfield.png");
	backgroundImage.resize(w, h);

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			std::cout << "Current progress: " << 100 * (y * w + x) / (w * h) << "%\r" << std::flush;
			glm::vec3 color(0.0f);
			for (uint i = 0; i < numSamples; ++i) {
				for (uint j = 0; j < numSamples; ++j) {
					float xOffset = randomFloat();
					float yOffset = randomFloat();
					float sample_x = x + (i + xOffset) / numSamples;
					float sample_y = y + (j + yOffset) / numSamples;
					// std::cout << xOffset << " " << yOffset << std::endl;
					glm::vec3 direction = getDirection(eye, view, up, fovy, w, h, sample_x, sample_y);
					// glm::vec3 direction = getDirection(eye, view, up, fovy, w, h, x, y);
					Ray ray(eye, direction, 0.0, RayType::Primary);
					IntersectionData data;
					if (root->intersect(ray, data)) {
						color += getColor(root, data, eye, ambient, lights);
					} else {
						color += glm::vec3(backgroundImage(x, y, 0), backgroundImage(x, y, 1), backgroundImage(x, y, 2));
					}
				}
			}
			color /= (numSamples * numSamples);
			// Red: 
			image(x, y, 0) = (double)color.x;
			// Green: 
			image(x, y, 1) = (double)color.y;
			// Blue: 
			image(x, y, 2) = (double)color.z;
			std::cout << "\b";
		}
	}
}

glm::vec3 getDirection(const glm::vec3& eye, const glm::vec3& view, const glm::vec3& up, double fovy,
					   size_t width, size_t height, float x, float y) {
	glm::vec3 lookat = glm::normalize(view);
	glm::vec3 right = glm::normalize(glm::cross(lookat, up));
	glm::vec3 camera_up = glm::normalize(glm::cross(right, lookat));
	float d = (height / 2.0) / tanf(glm::radians(fovy) * 0.5);
	glm::vec3 camera_to_center = lookat * d;
	glm::vec3 camera_to_tl = camera_to_center - right * (width / 2.0) + camera_up * (height / 2.0);

	glm::vec3 direction = glm::normalize(camera_to_tl + right * x - camera_up * y);
	return direction;
}

glm::vec3 getColor(SceneNode * root, IntersectionData& data, const glm::vec3 & eye, const glm::vec3 & ambient, const std::list<Light *> & lights) {
	glm::vec3 color(0.0f);
	PhongMaterial* material = dynamic_cast<PhongMaterial*>(data.material);
	color += ambient * material->getDiffuse();
	glm::vec3 surfaceNorm = glm::normalize(data.normal);
	// glm::vec3 surfaceNorm = data.normal;
	// std::cout << surfaceNorm.x << " " << surfaceNorm.y << " " << surfaceNorm.z << std::endl;
	glm::vec3 viewDir = glm::normalize(eye - data.position);
	for (Light* light: lights) {
		glm::vec3 lightDir = glm::normalize(light->position - data.position);
		Ray shadowRay(data.position + lightDir * EPSILON1, lightDir, EPSILON2, RayType::Shadow);
		IntersectionData shadowData;
		if (root->intersect(shadowRay, shadowData)) {
			continue;
		}
		glm::vec3 reflection = glm::normalize(-lightDir + 2 * glm::dot(lightDir, surfaceNorm) * surfaceNorm);
		double r = glm::length(light->position - data.position);
		glm::vec3 attenuatedLight = light->colour / (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r);

		color += material->getDiffuse() * std::max(0.0f, glm::dot(surfaceNorm, lightDir)) * attenuatedLight;
		
		color += material->getSpecular() * glm::pow(glm::max(0.0f, glm::dot(reflection, viewDir)), material->getShininess()) * attenuatedLight;

	}
	return color;
}

float randomFloat() {
    return dis(gen);
}
