#pragma once
#include <vector>
#include "particle.h"
#include "particle_simulator.h"

namespace idealgas {

class Histogram {
 public:
  
  /**
   * Constructs a histogram based on the mass of a particle 
   */
  Histogram(double mass);

  /**
   * Fills the bins with all the particles in the range 
   * @param particles the list of particles in the simulator that have the 
   * given mass
   */
  void FillBins(const std::vector<Particle> &particles);

  /**
   * Finds all the particles that have a given mass
   * @param particles the list of particles in the simulator that have the 
   * given mass 
   */
  std::vector<Particle> FindAllParticlesWithMass(const std::vector<Particle>&
      particles) const;
  
  /**
   * Draws the histogram
   * @param position the position on the GUI to be drawn at
   * @param particles the list of particles from the Particle simulator
   */
  void Draw(size_t position, const std::vector<Particle> &particles);

  const std::vector<size_t> &GetBins() const;

 private:
  std::vector<Particle> particles_;
  std::vector<size_t> bins_;
  double mass_;
  double lower_bound_;
  
  // These vectors represent the corners of the histogram 
  glm::vec2 vertical_axis_top_left_;
  glm::vec2 vertical_axis_bottom_left_;
  glm::vec2 horizontal_axis_bottom_left_;
  glm::vec2 horizontal_axis_bottom_right_;
  
  
  // This is the number of bars you want on the histogram
  const static size_t kNumberOfPartitions = 10;
  
  // The max speed for the histogram. Play around with this number if you 
  // have bigger particles with higher speeds 
  const static size_t kMaxSpeed = 50;
  
  // Sets the bounds for drawing the histogram 
  const static size_t kXUpperBound = ParticleSimulator::kXLowerBound * .9; 
  const static size_t kXLowerBound = ParticleSimulator::kXLowerBound * .1;
  
  // The height of a histogram 
  const static size_t kHeight = ParticleSimulator::kYUpperBound * .25;
  
  /**
   * Finds all the particles in a given speed range
   * @param min_speed the min speed for the range
   * @param max_speed the max speed for the range
   * @return the count of all the particles in the given range
   */
  size_t FindAllParticlesInSpeedRange(const std::vector<Particle> &particles, 
                                      double min_speed, double max_speed) const;
  
  /**
   * Updates the histogram by continuously drawing rectangles to match the 
   * number of particles in a given speed range
   */
  void UpdateHistogram();

  /**
   * Draws the X Axis labels for the histogram
   */
  void DrawXAxisLabels();
  
  /**
   * Draws the Y Axis labels for the histogram
   */
  void DrawYAxisLabels();
  
  /**
   * Draws the title for the histogram
   */
  void DrawTitle();
  
  /**
   * Draws the axis titles for the histogram
   */
  void DrawAxisTitles();
  
};

} // namespace idealgas
