#pragma once
#include "particle.h"
#include <vector>

namespace idealgas {

class ParticleSimulator {
 public:
  
  /**
   * Updates the simulation
   */
  void Update();
  
  /**
   * Adds particles to the simulation that spawn at random locations with random
   * initial velocities
   * @param amount amount the amount of particles desired to add
   * @param radius the radius of the particles added
   * @param mass the mass of the particles added
   * @param color  the color of the particles added 
   */
  void AddParticles(size_t amount, double radius, double mass, const std::string& color);
  
  /**
   * Overload method that allows user to specify the spawn location and 
   * initial velocities when adding particles
   * @param amount the amount of particles desired to add
   * @param radius the radius of the particles added
   * @param mass the mass of the particles added
   * @param color the color of the particles added 
   * @param x_coord the spawn x coordinate of the pixel
   * @param y_coord the spawn y coordinate of the pixel
   * @param initial_x_vel the initial horizontal velocity
   * @param initial_y_vel the initial vertical velocity
   */
  void AddParticles(size_t amount, double radius, double mass,
                    const std::string& color, size_t x_coord, size_t y_coord,
                    double initial_x_vel, double initial_y_vel);
  
  /**
   * Draws the particles onto the simulation
   */
  void Draw();
  
  /**
   * Speeds up all the particles
   */
  void SpeedUp();
  
  /**
   * Slows down all particles  
   */
  void SlowDown();

  const std::vector<Particle> &GetParticles() const;
  
  // Sets the window size of the GUI
  const static size_t kWindowSizeWidth = 1500;
  const static size_t kWindowSizeHeight = 800;
  
  // These constants establish the boundaries of the inner container 
  const static size_t kXLowerBound = kWindowSizeWidth * .3;
  const static size_t kXUpperBound = kWindowSizeWidth * .9;
  const static size_t kYLowerBound = kWindowSizeHeight * .1;
  const static size_t kYUpperBound = kWindowSizeHeight * .9;

 private:
  std::vector<Particle> particles_;
  constexpr static double kMinimumVelocity = 0.5;

  /**
   * Generates a random XY position in the range of the container boundaries
   * @return a random XY position pair
   */
  static std::pair<size_t, size_t> GenerateRandomXYPosition();
  
  /**
   * Generates a random XY initial velocity based on the constants defined 
   * above the maximum velocity the particle should have to prevent tunneling
   * @param radius the radius of the particle being generated to calculate 
   * @return a pair of the random XY initial velocities
   */
  static std::pair<double, double> GenerateRandomXYVelocity(double radius);

  /**
   * Checks if two particles are able to collide
   * @param particle1 the first particle to check
   * @param particle2 the second particle to check with
   * @return a bool on whether or not they can collide
   */
  bool CanCollide(const Particle& particle1, const Particle& particle2) const;
  
  /**
   * Allows the particles to collide and calculates their new velocities
   * @param particle1 the first particle that collides 
   * @param particle2 the second particle that collides 
   */
  void Collide(Particle& particle1, Particle& particle2);

  /**
   * Overload method that allows user to specify the spawn location and 
   * initial velocities when adding particles
   * @param amount the amount of particles desired to add
   * @param radius the radius of the particles added
   * @param mass the mass of the particles added
   * @param color the color of the particles added 
   * @param x_coord the spawn x coordinate of the pixel
   * @param y_coord the spawn y coordinate of the pixel
   * @param initial_x_vel the initial horizontal velocity
   * @param initial_y_vel the initial vertical velocity
   */
  void ValidateAddParticleArguments(double radius, double mass,
                                    size_t x_coord,
                                    size_t y_coord, double initial_x_vel, double 
                                    initial_y_vel) const;
  
};

} // namespace idealgas
