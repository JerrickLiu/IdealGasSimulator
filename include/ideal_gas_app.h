#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "particle_simulator.h"
#include "histogram.h"

namespace idealgas {

class IdealGasApp : public ci::app::App {
 public:
  IdealGasApp();

  void draw() override;
  void update() override;
  void setup() override;
  void keyDown(ci::app::KeyEvent event) override;
  
 private:
  ParticleSimulator particle_simulator_;
  std::vector<Histogram> histograms_;
  
  // Modify these constants to change the different particles' color
  const std::string kBigParticleColor = "blue";
  const std::string kMediumParticleColor = "green";
  const std::string kSmallParticleColor = "red";
  
  // Modify these constants to change the different particles' mass
  constexpr static double kBigParticleMass = 100;
  constexpr static double kMediumParticleMass = 25;
  constexpr static double kSmallParticleMass = 5;

  // Modify these constants to change the different particles' radii 
  constexpr static double kBigParticleRadius = 20;
  constexpr static double kMediumParticleRadius = 12;
  constexpr static double kSmallParticleRadius = 8;

  // Modify these constants to change the number of particles in the simulation
  const static size_t kBigParticleAmount = 25;
  const static size_t kMediumParticleAmount = 25;
  const static size_t kSmallParticleAmount = 25;
};

}  // namespace idealgas
