// // Termm--Fall 2023

// #pragma once

// #include <glm/glm.hpp>
// #include <iostream>
// #include <string>

// #include "Material.hpp"

// class ImageMaterial : public Material {
// public:
//   ImageMaterial(const glm::vec3& ka, const glm::vec3& ks, const double shininess, const std::string& img_name);
//   ImageMaterial(const glm::vec3& ka, const glm::vec3& kd, const glm::vec3& ks, const double shininess);
//   virtual ~ImageMaterial();
//   glm::vec3 getAmbient() const;
//   glm::vec3 getDiffuse(glm::vec3& pos) const;
//   glm::vec3 getSpecular() const;
//   double getShininess() const;
//   double getReflection() const;
//   double getTransparency() const;
//   double getIOR() const;
//   std::string getImgName() const;
//   bool isGlossyReflection() const;
//   bool isGlossyTransmission() const;
//   void setDiffuse(glm::vec3 kd);
//   void setReflection(double reflection, int glossy);
//   void setRefraction(double transparency, double IOR, int glossy);
//   void setPerlinNoise(Perlin* perlin, float scale);
//   friend std::ostream& operator<<(std::ostream& out, const ImageMaterial& material);

// // private:
//   glm::vec3 m_ka;
//   glm::vec3 m_kd;
//   glm::vec3 m_ks;
//   std::string img_name;

//   double m_shininess;
// };
