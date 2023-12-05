// Termm--Fall 2023

#include <glm/ext.hpp>
#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <unordered_set>
#include "A4.hpp"
#include "Random.hpp"

const double EPSILON1 = 5e-1;
const double EPSILON2 = 0;
const size_t numSamples = 50;
const size_t numGlossySamples = 5;
const size_t maxDepth = 10;
const float minValue = 0.0f;
const float maxValue = 0.9999999999f;
const float PI = 3.1415926f;
const float prob = 0.6f;
float fr_diffuse = 1.0 / glm::pi<float>();

Random randf(minValue, maxValue);

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
		const std::list<AreaLight *> & lights
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

	// for(const Light * light : lights) {
	// 	std::cout << "\t\t" <<  *light << std::endl;
	// }
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();
	size_t pixelCnt = h * w;

	Image backgroundImage;
	// backgroundImage.loadPng("sky.png");
	backgroundImage.loadPng("starfield.png");
	backgroundImage.resize(w, h);

	std::mutex mutex;
	std::condition_variable cv;
	std::atomic<int> progress{0}; 
	std::vector<std::future<std::vector<Pixel>>> m_futures;

	auto start_time = std::chrono::high_resolution_clock::now();
	
	// Start the progress printing thread
	std::thread progressThread([&progress, &pixelCnt]() {
		while (progress < pixelCnt) {
			std::cout << "\rProgress: " << progress * 100 / pixelCnt << "%" << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Adjust the sleep duration as needed
		}
	});

	for (uint y = 0; y < h; ++y) {
		auto future = std::async(std::launch::async | std::launch::deferred, [&, y]() 
		{
			std::vector<Pixel> pixels;
			for (uint x = 0; x < w; ++x) {
				glm::vec3 color(0.0f);
				for (uint i = 0; i < numSamples; ++i) {
					for (uint j = 0; j < numSamples; ++j) {
						float xOffset = randf.randomFloat();
						float yOffset = randf.randomFloat();
						float sample_x = x + (i + xOffset) / numSamples;
						float sample_y = y + (j + yOffset) / numSamples;
						// std::cout << xOffset << " " << yOffset << std::endl;
						glm::vec3 direction = getDirection(eye, view, up, fovy, w, h, sample_x, sample_y);
						// glm::vec3 direction = getDirection(eye, view, up, fovy, w, h, x, y);
						Ray ray(eye, direction, 0.0, RayType::Primary);
						IntersectionData data;
						if (root->intersect(ray, data)) {
							glm::vec3 inverseDir = -direction;
							Material* material = data.material;
							if (material->hasEmission()) {
								color += material->getDiffuse(data.position, data.uv, data.img_name);
							} else {
								color += material->getDiffuse(data.position, data.uv, data.img_name) * shade(root, data.position, inverseDir, data.normal, material, lights);
							}
						} else {
							color += glm::vec3(backgroundImage(x, y, 0), backgroundImage(x, y, 1), backgroundImage(x, y, 2));
						}
						// glm::vec3 c = getColor(root, ray, data, eye, ambient, lights, 1.0, 1);
						// if (c == glm::vec3(0.0)) {
						// 	color += glm::vec3(backgroundImage(x, y, 0), backgroundImage(x, y, 1), backgroundImage(x, y, 2));
						// } else {
						// 	color += c;
						// }
					}
				}
				
				color /= (numSamples * numSamples);
				pixels.emplace_back(x, y, color);
				progress++;
			}
			return pixels;
		});

		{
			std::lock_guard<std::mutex> lock(mutex);
			m_futures.push_back(std::move(future));
		}
	}

	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [&m_futures, &h] {
			return m_futures.size() == h;
		});
	}

	for (std::future<std::vector<Pixel>>& m_future : m_futures)
	{
		std::vector<Pixel> pixels = m_future.get();
		for (size_t i = 0; i < w; ++i) {
			uint x = pixels[i].x;
			uint y = pixels[i].y;
			glm::vec3& color = pixels[i].color;
			// Red: 
			image(x, y, 0) = (double)color.x;
			// Green: 
			image(x, y, 1) = (double)color.y;
			// Blue: 
			image(x, y, 2) = (double)color.z;
		}
	}

	progressThread.join();
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::cout << "\rTime taken by program: " << duration.count() << " microseconds" << std::endl;
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

glm::vec3 getColor(SceneNode * root, Ray& ray, IntersectionData& data, const glm::vec3 & eye, const glm::vec3 & ambient, const std::list<AreaLight *> & lights, double ni, int depth) {
	glm::vec3 color(0.0f);
	if (depth <= maxDepth && root->intersect(ray, data)) {
		Material* material = data.material;
		if (material->hasEmission()) {
			return material->getDiffuse(data.position, data.uv, data.img_name);
		}
		color = ambient * material->getAmbient(data.position, data.uv, data.img_name);
		glm::vec3 norm = glm::normalize(data.normal);
		// glm::vec3 norm = data.normal;
		// std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;
		glm::vec3 viewDir = glm::normalize(eye - data.position);
		glm::vec3 Cr(0.0f), Ct(0.0f);
		bool reflective = material->getReflection() > 0;
		bool transparent = material->getTransparency() > 0;
		
		// phong illumination model
		for (AreaLight* light: lights) {
			color += getShade(root, light, material, norm, viewDir, data);
		}
		
		// reflection
		if ( reflective ) {
			Cr = getReflection(root, material, viewDir, norm, ambient, data, lights, ni, depth);
		}

		// refraction
		double cosTheta = glm::dot(norm, viewDir);
		double nt = material->getIOR();
		if ( transparent ) {
			// std::string b = "yes";
			// if (transparent == 0) {
			// 	b = "no";
			// }
			Ct = getRefraction(root, material, viewDir, norm, ambient, data, lights, ni, nt, depth, cosTheta, transparent);
			// if (Ct == glm::vec3(0.0)) {
			// 	std::string a ="yes";
			// 	if (transparent == 0) {
			// 		a = "no";
			// 	}
			// 	std::cout << "error" << " " << b << " " << a << std::endl;
			// }
		}
	
		// Fresnel Effect
		double reflectance = 0;
		if (reflective && transparent) {
			double R0 = glm::pow((nt - 1) / (nt + 1) , 2);
			double reflectance = R0 + (1 - R0) * glm::pow(1 - cosTheta, 5);	
			// if (nt != 1.0) {
			// 	std::cout << nt <<" " << R0 << " " << cosTheta << " " << reflectance << std::endl;
			// }
		} else if (reflective) {
			reflectance = 1;
		} else if (transparent) {
			// std::cout << Ct.x << " " <<  Ct.y << " " << Ct.z << std::endl;
			reflectance = 0;
		}
		// std::cout << tcc << " "  << " " << reflectance << std::endl;
		color += reflectance * Cr + (1 - reflectance) * Ct;
	}
	return color;
}

glm::vec3 getShade(SceneNode* root, AreaLight* light, Material* material, glm::vec3& norm, glm::vec3& viewDir, IntersectionData& data) {
	double intensity = 0.0;
	glm::vec3 arealightColor(0.0f);
	for (size_t i = 0; i < light->u_steps; ++i) {
		for (size_t j = 0; j < light->v_steps; ++j){
			for (size_t k = 0; k < light->w_steps; ++k) {
				float u_offset = randf.randomFloat();
				float v_offset = randf.randomFloat();
				float w_offset = randf.randomFloat();
				glm::vec3 lightPos = light->position + (i + u_offset) * light->u_vec + (j + v_offset) * light->v_vec + (k + w_offset) * light->w_vec;
				glm::vec3 lightDir = glm::normalize(lightPos - data.position);
				Ray shadowRay(data.position + lightDir * EPSILON1, lightDir, EPSILON2, RayType::Shadow);
				IntersectionData shadowData;
				double r = glm::length(lightPos - data.position);

				if (root->intersect(shadowRay, shadowData) && !shadowData.material->hasEmission()) {
					continue;
				}
				// if (root->intersect(shadowRay, shadowData) && glm::length(shadowData.position - data.position) < r) {
				// 	continue;
				// }
				intensity += 1.0;
				glm::vec3 lightReflectedDir = glm::normalize(-lightDir + 2 * glm::dot(lightDir, norm) * norm);
				glm::vec3 attenuatedLight = light->colour / (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r);
				arealightColor += material->getDiffuse(data.position, data.uv, data.img_name) * std::max(0.0f, glm::dot(norm, lightDir)) * attenuatedLight;
				arealightColor += material->getSpecular() * glm::pow(glm::max(0.0f, glm::dot(lightReflectedDir, viewDir)), material->getShininess()) * attenuatedLight;
			}
		}
	}
	size_t numLightSamples = light->u_steps * light->v_steps * light->w_steps;
	intensity /= numLightSamples;
	arealightColor /= numLightSamples;
	return intensity * arealightColor;
}

glm::vec3 getReflection(SceneNode * root, Material* material, const glm::vec3 & viewDir, const glm::vec3 & norm, const glm::vec3 & ambient, IntersectionData& data, const std::list<AreaLight *> & lights, double ni, int depth) {
	const double blur = material->getGlossyReflection();
	const size_t glossySamples = blur > 0 ? numGlossySamples : 1;
	glm::vec3 reflectionColor(0.0f);
	glm::vec3 reflection = glm::normalize(-viewDir - 2 * glm::dot(-viewDir, norm) * norm);
	glm::vec3 t = reflection;
	if (std::abs(t.x) <= std::abs(t.y) && std::abs(t.x) <= std::abs(t.z)) {
		t.x = 1;
	} else if (std::abs(t.y) <= std::abs(t.x) && std::abs(t.y) <= std::abs(t.z)) {
		t.y = 1;
	} else {
		t.z = 1;
	}
	glm::vec3 u = glm::normalize(glm::cross(t, reflection));
	glm::vec3 v = glm::cross(reflection, u);
	for (size_t i = 0; i < glossySamples; ++i) {
		float u_offset = randf.randomFloat();
		float v_offset = randf.randomFloat();
		float u_ = - blur/2 + u_offset * blur;
		float v_ = - blur/2 + v_offset * blur;
		glm::vec3 reflectedDir = glm::normalize(reflection + u_ * u + v_ * v);
		Ray reflectedRay(data.position + reflectedDir * EPSILON1, reflectedDir, EPSILON2, RayType::Reflection);
		IntersectionData reflectionData;
		reflectionColor += material->getReflection() * getColor(root, reflectedRay, reflectionData, data.position, ambient, lights, ni, depth + 1);
	}
	reflectionColor /= glossySamples;
	return reflectionColor;
}

glm::vec3 getRefraction(SceneNode * root, Material* material, const glm::vec3 & viewDir, glm::vec3 & norm, const glm::vec3 & ambient, IntersectionData& data, const std::list<AreaLight *> & lights, double ni, double& nt, int depth, double& cosTheta, bool& transparent) {
	double cosVN = cosTheta;
	glm::vec3 refractedColor(0.0f);
	if (cosVN <= 0) {
		norm = -norm;
		nt = 1.0;
		cosVN = -cosVN;
	}
	double nr = ni / nt;
	double val  = 1 - nr * nr * (1 - cosVN * cosVN);
	if (val > 0) {
		glm::vec3 refraction = glm::normalize((nr * cosVN - glm::sqrt(val)) * norm - nr * viewDir);
		if (cosTheta <= 0) {
			cosTheta = glm::dot(-norm, refraction);
		}
		const double blur = material->getGlossyTransmission();
		const size_t glossySamples = blur > 0 ? numGlossySamples : 1;
		glm::vec3 t = refraction;
		if (std::abs(t.x) <= std::abs(t.y) && std::abs(t.x) <= std::abs(t.z)) {
			t.x = 1;
		} else if (std::abs(t.y) <= std::abs(t.x) && std::abs(t.y) <= std::abs(t.z)) {
			t.y = 1;
		} else {
			t.z = 1;
		}
		glm::vec3 u = glm::normalize(glm::cross(t, refraction));
		glm::vec3 v = glm::cross(refraction, u);
		for (size_t i = 0; i < glossySamples; ++i) {
			float u_offset = randf.randomFloat();
			float v_offset = randf.randomFloat();
			float u_ = - blur/2 + u_offset * blur;
			float v_ = - blur/2 + v_offset * blur;
			glm::vec3 refractedDir = glm::normalize(refraction + u_ * u + v_ * v);
			Ray refractedRay(data.position + refractedDir * EPSILON1, refractedDir, EPSILON2, RayType::Refraction);
			IntersectionData refractionData;
			refractedColor += material->getTransparency() * getColor(root, refractedRay, refractionData, data.position, ambient, lights, nt, depth + 1);
		}
		refractedColor /= glossySamples;
	} else {
		// total internal reflection
		transparent = false;
	}
	return refractedColor;
}

glm::vec3 shade(SceneNode * root, glm::vec3& pos, glm::vec3& direction, glm::vec3& norm, Material* m, const std::list<AreaLight *> & lights) {
	glm::vec3 directLight(0.0f);
	glm::vec3 indirectLight(0.0f);
	IntersectionData lightData;
	float lightPdf = sampleLight(lightData, lights);
	glm::vec3 lightPos = lightData.position;
	glm::vec3 lightColor = lightData.colour;
	glm::vec3 lightDir = glm::normalize(lightPos - pos);

	// if (pos.y > 26) {
	// 	std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
	// }
	// std::cout << lightDir.x << " " << lightDir.y << " " << lightDir.z << std::endl;
	float r = glm::length(lightPos - pos);

	glm::vec2 uv(0.0f);
	std::string name = "";

	Ray directRay(pos + lightDir * EPSILON1, lightDir, EPSILON2, RayType::Direct);
	IntersectionData directData;
	root->intersect(directRay, directData);

	// test 1
	// if (glm::length(directData.position - pos) - r > 1e-1 ) {
	// 	directLight = lightColor * fr_diffuse * m->getDiffuse(pos, uv, name) * glm::dot(lightDir, norm) * std::max(0.0f, glm::dot(-lightDir, directData.normal)) / (r * r) / (lightPdf);
	// }
	if (abs(glm::length(directData.position - pos) - r ) <= 1e-1 ) {
		directLight = lightColor * fr_diffuse * m->getDiffuse(pos, uv, name) * glm::dot(lightDir, norm) * std::max(0.0f, glm::dot(-lightDir, directData.normal)) / (r * r) / (lightPdf);
	}

	float p = randf.randomFloat();
	if (p <= prob) {
		glm::vec3 indirectDir = UnifSampleHemisphere(norm);
		float unifPdf = 1 / (2 * PI);
		Ray indirectRay(pos + indirectDir * EPSILON1, indirectDir, EPSILON2, RayType::Indirect);

		IntersectionData indirectData;
		if (root->intersect(indirectRay, indirectData) && !indirectData.material->hasEmission()) {
			// if (indirectData.position.y > 26) {
			// 	std::cout << pos.x << " " << pos.y << " " << pos.z << " " << indirectDir.x << " " << indirectDir.y << " " << indirectDir.z << " " << norm.x << " " << norm.y << " " << norm.z << std::endl;
			// }
			Material* material = indirectData.material;
			glm::vec3 inverseDir = -indirectDir;
			indirectLight = fr_diffuse * m->getDiffuse(pos, uv, name) * shade(root, indirectData.position, inverseDir, indirectData.normal, material, lights)
			* glm::dot(indirectDir, norm) / unifPdf / prob;
		}
	}
	// std::cout << directLight.x << " " << directLight.y << " " << directLight.z << std::endl;
	return directLight + indirectLight;
}

float sampleLight(IntersectionData& data, const std::list<AreaLight *> & lights) {
	// how to sample light
	AreaLight* light = lights.front();
	glm::vec3 u = randf.randomFloat() * light->u_steps * light->u_vec;
	glm::vec3 v = randf.randomFloat() * light->v_steps * light->v_vec;
	glm::vec3 w = randf.randomFloat() * light->w_steps * light->w_vec;
	data.position = light->position + u + v + w;
	data.colour = light->colour;
	data.initPos = light->position;
	
	return 1.0f / light->getArea();
}

glm::vec3 UnifSampleHemisphere(const glm::vec3& normal)
{
    float cosTheta = randf.randomFloat();
    float sinTheta = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
    float phi = 2.0f * PI * randf.randomFloat();
    glm::vec3 tangentSpaceDir(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
	glm::mat3 tangentSpaceMatrix = GetTangentSpace(normal);

    return glm::normalize(tangentSpaceMatrix * tangentSpaceDir);
}

glm::mat3 GetTangentSpace(const glm::vec3& normal)
{
    glm::vec3 helper(1, 0, 0);
    if (std::abs(normal.x) > 0.99f)
        helper = glm::vec3(0, 0, 1);

    glm::vec3 tangent = glm::normalize(glm::cross(normal, helper));
    glm::vec3 binormal = glm::normalize(glm::cross(normal, tangent));
	return glm::mat3(tangent, binormal, normal);
}
