// Termm--Fall 2023

#pragma once


#include <glm/glm.hpp>
#include "AreaLight.hpp"
#include "IntersectionData.hpp"
#include "Material.hpp"
#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "ray.hpp"

struct Pixel {
	uint x;
	uint y;
	glm::vec3 color;

	Pixel(uint x, uint y, glm::vec3& color) : x(x), y(y), color(color) {}
};

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
);

glm::vec3 getDirection(const glm::vec3& eye, const glm::vec3& view, const glm::vec3& up, double fovy,
					   size_t width, size_t height, float x, float y);

glm::vec3 getColor(SceneNode * root, Ray& ray, IntersectionData& data, const glm::vec3& eye, const glm::vec3 & ambient, const std::list<AreaLight *> & lights, double ni, int depth);

glm::vec3 getShade(SceneNode* root, AreaLight* light, Material* material, glm::vec3& norm, glm::vec3& viewDir, IntersectionData& data);

glm::vec3 getReflection(SceneNode * root, Material* material, const glm::vec3 & viewDir, const glm::vec3 & norm, const glm::vec3 & ambient, IntersectionData& data, const std::list<AreaLight *> & lights, double ni, int depth);

glm::vec3 getRefraction(SceneNode * root, Material* material, const glm::vec3 & viewDir, glm::vec3 & norm, const glm::vec3 & ambient, IntersectionData& data, const std::list<AreaLight *> & lights, double ni, double& nt, int depth, double& cosTheta, bool& transparent);

glm::vec3 getColorPT(SceneNode * root, Ray& ray, IntersectionData& data, const glm::vec3 & eye, double ni, int depth);

float sampleLight(IntersectionData& data, const std::list<AreaLight *> & lights);

glm::vec3 UnifSampleHemisphere(const glm::vec3& normal);

glm::mat3 GetTangentSpace(const glm::vec3& normal);

glm::vec3 shade(SceneNode * root, glm::vec3& pos, glm::vec3& direction, glm::vec3& norm, Material* m, const std::list<AreaLight *> & lights);

glm::vec3 randomVec3();

glm::vec3 randomDirection(glm::vec3 n);