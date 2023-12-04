// // Termm--Fall 2023

// #include "ImageMaterial.hpp"

// ImageMaterial::ImageMaterial(
// 	const glm::vec3& ka, const glm::vec3& ks, const double shininess, const std::string& img_name )
// 	: m_ka(ka)
// 	, m_ks(ks)
// 	, m_shininess(shininess)
//     , img_name(img_name)
// {}

// ImageMaterial::ImageMaterial(
// 	const glm::vec3& ka, const glm::vec3& kd, const glm::vec3& ks, const double shininess)
// 	: m_ka(ka)
// 	, m_kd(kd)
// 	, m_ks(ks)
// 	, m_shininess(shininess)
// {}

// ImageMaterial::~ImageMaterial()
// {}

// glm::vec3 ImageMaterial::getAmbient() const {
// 	return m_ka;
// }

// glm::vec3 ImageMaterial::getDiffuse(glm::vec3& pos) const {
// 	return m_kd;
// }

// glm::vec3 ImageMaterial::getSpecular() const {
// 	return m_ks;
// }

// double ImageMaterial::getShininess() const {
// 	return m_shininess;
// }

// double ImageMaterial::getReflection() const {
// 	return 0.0;
// }

// double ImageMaterial::getTransparency() const {
// 	return 0.0;
// }

// double ImageMaterial::getIOR() const {
// 	return 0.0;
// }

// std::string ImageMaterial::getImgName() const {
// 	return img_name;
// }

// void ImageMaterial::setDiffuse(glm::vec3 kd) {
// 	m_kd = kd;
// }

// void ImageMaterial::setReflection(double reflection, int glossy) {
// 	return;
// }

// void ImageMaterial::setPerlinNoise(Perlin* perlin, float scale) {
// 	return;
// }

// void ImageMaterial::setRefraction(double transparency, double IOR, int glossy) {
// 	return;
// }

// bool ImageMaterial::isGlossyReflection() const {
// 	return false;
// }

// bool ImageMaterial::isGlossyTransmission() const {
// 	return false;
// }

// std::ostream& operator<<(std::ostream& out, const ImageMaterial& material) {
// 	out << "ImageMaterial: " << material.m_ka.x << " " << material.m_ka.y << " " << material.m_ka.z << " " << material.m_ks.x << " " << material.m_ks.y << " " << material.m_ks.z << " " << material.m_shininess << " " << material.img_name << std::endl;
// 	return out;
// }