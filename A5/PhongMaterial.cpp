// Termm--Fall 2023

#include "PhongMaterial.hpp"
#include <iostream>

PhongMaterial::PhongMaterial(
	const glm::vec3& ka, const glm::vec3& kd, const glm::vec3& ks, double shininess)
	: m_ka(ka)
	, m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_reflection(0.0)
	, m_glossy_reflection(0.0)
	, m_transparency(0.0)
	, m_IOR(0.0)
	, m_scale(1.0)
	, m_glossy_refraction(0.0)
	, m_perlin(nullptr)
	, m_light(nullptr)
	, m_image(nullptr)
{
}

PhongMaterial::~PhongMaterial()
{
}

glm::vec3 PhongMaterial::getAmbient(glm::vec3& pos, glm::vec2&uv, std::string& img_name) {
	return m_ka * getDiffuse(pos, uv, img_name);
}

glm::vec3 PhongMaterial::getDiffuse(glm::vec3& pos, glm::vec2&uv, std::string& img_name) {
	if (m_perlin != nullptr) {
		glm::vec3 pos_ = pos * m_scale;
		// return glm::vec3(1.0f) * m_perlin->turbulence(pos * m_scale, 7);
		float pattern = m_perlin->isVertical() ? pos_.x : pos_.y;
		return glm::vec3(1.0f) * 0.5f * (1.0f + std::sin(3 * pattern + 5 * m_perlin->turbulence(pos_, 7)));
	} else if (m_light != nullptr) {
		return m_light->colour;
	} else if (m_image != nullptr) {
		Image& image = *m_image;
		double x_ = uv.x;
		double y_ = uv.y;
		uint x = (x_ - std::floor(x_)) * image.width();
		uint y = (y_ - std::floor(y_)) * image.height();
		return glm::vec3(image(x, y, 0), image(x, y, 1), image(x, y, 2));
	}
	else if (m_images.size() > 0) {
		Image& image = m_images[img_name];
		double x_ = uv.x - std::floor(uv.x);
		double y_ = uv.y - std::floor(uv.y);
		uint x = x_ * image.width();
		uint y = (1 - y_) * image.height();
		return glm::vec3(image(x, y, 0), image(x, y, 1), image(x, y, 2));
	} 
	return m_kd;
}

glm::vec3 PhongMaterial::getSpecular() const {
	return m_ks;
}

double PhongMaterial::getShininess() const {
	return m_shininess;
}

double PhongMaterial::getReflection() const {
	return m_reflection;
}

double PhongMaterial::getTransparency() const {
	return m_transparency;
}

double PhongMaterial::getIOR() const {
	return m_IOR;
}

double PhongMaterial::getGlossyReflection() const {
	return m_glossy_reflection;
}

double PhongMaterial::getGlossyTransmission() const {
	return m_glossy_refraction;
}

bool PhongMaterial::hasEmission() const {
	return m_light != nullptr;
}

void PhongMaterial::setReflection(double reflection, double glossy) {
	m_reflection = reflection;
	m_glossy_reflection = glossy;
}

void PhongMaterial::setRefraction(double transparency, double IOR, double glossy) {
	m_transparency = transparency;
	m_IOR = IOR;
	m_glossy_refraction = glossy;
}

void PhongMaterial::setPerlinNoise(Perlin* perlin, float scale) {
	m_scale = scale;
	m_perlin = perlin;
}

void PhongMaterial::setImages(std::unordered_map<std::string, std::string>& mat_img_map) {
	for (auto& entry: mat_img_map) {
		std::string img_name = entry.second;
		if (m_images.count(img_name) == 0) {
			std::string img_file = "Assets/" + img_name;
			Image image;
			image.loadPng(img_file);
			m_images[img_name] = image;
		}
	}
}

void PhongMaterial::setIllumination(AreaLight* light) {
	m_light = light;
}

void PhongMaterial::setImage(Image* image) {
	m_image = image;
}