// Termm--Fall 2023

#pragma once

#include <random>
#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

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
);

glm::vec3 getDirection(const glm::vec3& eye, const glm::vec3& view, const glm::vec3& up, double fovy,
					   size_t width, size_t height, float x, float y);

glm::vec3 getColor(SceneNode * root, IntersectionData& data, const glm::vec3& eye, const glm::vec3 & ambient, const std::list<Light *> & lights);

void getDirection(
	glm::vec3 eye, glm::vec3 view, glm::vec3 up,
	double fovy, size_t w, size_t h, float x, float y,
	glm::vec3& direction);

void getDirection(
	glm::vec3 eye, glm::vec3 view, glm::vec3 up,
	double fovy, size_t w, size_t h, float x, float y,
	glm::vec3& direction);

std::random_device rd;
std::mt19937 gen(rd());
const float minValue = -0.5f;
float maxValue = 0.5f;
std::uniform_real_distribution<float> dis(minValue, maxValue);
float randomFloat();