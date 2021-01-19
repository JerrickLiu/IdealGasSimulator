#pragma once
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "../../../include/glm/glm.hpp"
#include <string>

namespace idealgas {

class Particle {
 public:
  /**
   * Constructs a particle given the location and velocity as vectors, the 
   * radius, the mass, and the color 
   */
  Particle(const glm::vec2& location, const glm::vec2& velocity, double 
  radius, double mass, const std::string& color);
  
  /**
   * Draws the actual particle to the screen
   */
  void DrawParticle();
  
  /**
   * Updates the particle's position and velocity 
   */
  void Update();
  
  /**
   * Speeds up the particle
   */
  void SpeedUp();
  
  /**
   * Slows down the particle
   */
  void SlowDown();
  
  void SetVelocity(const glm::vec2 &velocity);
  
  const glm::vec2 &GetPosition() const;
  const glm::vec2 &GetVelocity() const;
  double GetRadius() const;
  double GetMass() const;
  const std::string &GetColor() const;

 private:
  glm::vec2 position_;
  glm::vec2 velocity_;
  double mass_;
  std::string color_;
  double radius_;
};

} // namespace idealgas