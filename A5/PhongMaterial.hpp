// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& ka, const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual ~PhongMaterial();
  glm::vec3 getAmbient(glm::vec3& pos, glm::vec2&uv, std::string& img_name);
  glm::vec3 getDiffuse(glm::vec3& pos, glm::vec2&uv, std::string& img_name);
  glm::vec3 getSpecular() const;
  double getShininess() const;
  double getReflection() const;
  double getTransparency() const;
  double getIOR() const;
  double getGlossyReflection() const;
  double getGlossyTransmission() const;
  bool hasEmission() const;
  void setPerlinNoise(Perlin* perlin, float scale);
  void setReflection(double reflection, double glossy);
  void setRefraction(double transparency, double IOR, double glossy);
  void setImage(Image* image);
  void setImages(std::unordered_map<std::string, std::string>& mat_img_map);
  void setIllumination(AreaLight* light);

private:
  glm::vec3 m_ka;
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflection;
  double m_transparency;
  double m_IOR;
  double m_glossy_reflection;
  double m_glossy_refraction;

  float m_scale;
  AreaLight* m_light;
  Image* m_image;
  Perlin* m_perlin;
  
  std::unordered_map<std::string, Image> m_images;
};
