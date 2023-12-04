// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include "AreaLight.hpp"
#include "Image.hpp"
#include "Perlin.hpp"


class Material {
public:
  virtual ~Material();
  virtual glm::vec3 getAmbient(glm::vec3& pos, glm::vec2&uv, std::string& img_name) = 0;
  virtual glm::vec3 getDiffuse(glm::vec3& pos, glm::vec2&uv, std::string& img_name) = 0;
  virtual glm::vec3 getSpecular() const = 0; 
  virtual double getShininess() const = 0;
  virtual double getReflection() const = 0;
  virtual double getGlossyReflection() const = 0;
  virtual double getGlossyTransmission() const = 0;
  virtual bool hasEmission() const = 0;
  virtual double getTransparency() const = 0;
  virtual double getIOR() const = 0;
  virtual void setReflection(double reflection, double glossy) = 0;
  virtual void setRefraction(double transparency, double IOR, double glossy) = 0;
  virtual void setPerlinNoise(Perlin* perlin, float scale) = 0;
  virtual void setImages(std::unordered_map<std::string, std::string>& mtl_img_map) = 0;
  virtual void setImage(Image* image) = 0;
  virtual void setIllumination(AreaLight* light) = 0;

protected:
  Material();
};
