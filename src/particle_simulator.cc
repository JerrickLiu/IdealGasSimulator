#include <particle_simulator.h>
#include <random>

namespace idealgas {

void ParticleSimulator::Update() {
  for (size_t i = 0; i < particles_.size(); i++) {
    for (size_t j = i + 1; j < particles_.size(); j++) {
      if (CanCollide(particles_[i], particles_[j])) {
        Collide(particles_[i], particles_[j]);
      }
    }
    particles_[i].Update();
  }
}

bool ParticleSimulator::CanCollide(const Particle& particle1, const Particle&
particle2) const {
  
  // Checks if the distance between the particles is less than the sum of 
  // their radii
  if (glm::distance(particle1.GetPosition(), particle2.GetPosition()) < 
  particle1.GetRadius() + particle2.GetRadius()) {
    
    glm::vec2 v1_difference = particle1.GetVelocity() - particle2.GetVelocity();
    glm::vec2 x1_difference = particle1.GetPosition() - particle2.GetPosition();

    // Check if they are moving toward each other. If not, then we return 
    // and not collide
    if (dot(v1_difference, x1_difference) < 0) {
      return true;
    }
  }
  return false;
}

void ParticleSimulator::Collide(Particle &particle1, Particle &particle2) {
  
  float p1_mass = particle1.GetMass();
  float p2_mass = particle2.GetMass();

  // Calculates v1 - v2
  glm::vec2 v1_difference = particle1.GetVelocity() - particle2.GetVelocity();
  
  // Calculates x1 - x2
  glm::vec2 x1_difference = particle1.GetPosition() - particle2.GetPosition();
  
  // Finds the magnitude 
  float magnitude1 = glm::length(x1_difference);
  
  // Same thing for particle 2
  glm::vec2 v2_difference = particle2.GetVelocity() - particle1.GetVelocity();
  glm::vec2 x2_difference = particle2.GetPosition() - particle1.GetPosition();
  float magnitude2 = glm::length(x2_difference);
  
  // Calculate the mass ratios between the particles 
  float p1_mass_ratio = ((2 * p2_mass) / (p1_mass + p2_mass));
  float p2_mass_ratio = ((2 * p1_mass) / (p1_mass + p2_mass));
  
  // Equations from the assignment specs
  glm::vec2 p1_new_vel = particle1.GetVelocity() -  p1_mass_ratio * ((float) 
      (dot(v1_difference, x1_difference) / pow(magnitude1, 2)) * x1_difference);
  
  glm::vec2 p2_new_vel = particle2.GetVelocity() - p2_mass_ratio * ((float) 
      (dot(v2_difference, x2_difference) / pow(magnitude2, 2)) * x2_difference);
  
  particle1.SetVelocity(p1_new_vel);
  particle2.SetVelocity(p2_new_vel);
}

std::pair<size_t, size_t> ParticleSimulator::GenerateRandomXYPosition() {
  std::random_device rd;
  
  // Seeds the random generator
  std::mt19937 mt(rd());
  
  std::uniform_int_distribution<size_t> x_pos_distribution(
      kXLowerBound + 2, kXUpperBound - 2);
  std::uniform_int_distribution<size_t> y_pos_distribution(
      kYLowerBound + 2, kYUpperBound - 2);
  return std::make_pair(x_pos_distribution(mt), y_pos_distribution(mt));
}

std::pair<double, double> ParticleSimulator::GenerateRandomXYVelocity(double radius) {
  std::random_device rd;
  
  // Seeds the random generator
  std::mt19937 mt(rd());
  
  // We half the radius to get the maximum range for the 
  // particles velocity to prevent tunneling
  std::uniform_int_distribution<double> x_vel_distribution(kMinimumVelocity, 
                                                           radius / 2);
  std::uniform_int_distribution<double> y_vel_distribution(kMinimumVelocity, 
                                                           radius / 2);
  return std::make_pair(x_vel_distribution(mt), y_vel_distribution(mt));
}

void ParticleSimulator::AddParticles(size_t amount, double radius, double mass,
                                     const std::string& color) {

  
  if (radius < 0.1) {
    throw std::invalid_argument("Please make sure the radius of the particles"
                                " is at least 1!");
  }
  
  for (size_t i = 0; i < amount; i++) {
    std::pair<size_t, size_t> xy_position = GenerateRandomXYPosition();
    std::pair<double, double> xy_velocity = GenerateRandomXYVelocity(radius);
    particles_.emplace_back(glm::vec2(xy_position.first, xy_position.second), 
                            glm::vec2(xy_velocity.first, xy_velocity.second),
                            radius, mass, color);
  }
}


void ParticleSimulator::AddParticles(size_t amount, double radius,
                                     double mass, const std::string& color,
                                     size_t x_coord, size_t y_coord,
                                     double initial_x_vel, double initial_y_vel) {
  
  ValidateAddParticleArguments(radius, mass, x_coord, y_coord, 
                               initial_x_vel, 
                               initial_y_vel);
    
  for (size_t i = 0; i < amount; i++) {
    particles_.emplace_back(glm::vec2(x_coord, y_coord),
                            glm::vec2(initial_x_vel, initial_y_vel), radius,
                            mass, color);
  }
}
void ParticleSimulator::ValidateAddParticleArguments(double radius,
                                                     double mass,
                                                     size_t x_coord,
                                                     size_t y_coord,
                                                     double initial_x_vel,
                                                     double initial_y_vel) 
                                                     const {
  
  // Radius should not be less than 0.1 
  if (radius < 0.1) {
    throw std::invalid_argument("Please make sure the radius of the particles"
                                " is at least 0.1!");

  } else if (mass < 0.1) {
    throw std::invalid_argument("Please make sure the mass of the particles"
                                " is at least 0.1!");
    // Initial velocity should not be 0
  } else if (initial_x_vel == 0 && initial_y_vel == 0) {
    throw std::invalid_argument("Please make sure the initial velocity of the"
                                " particles is not 0!");
    
    // If particle velocity is greater than the half its radius, tunneling 
    // may occur so we throw an error if that's the parameter 
  } else if (abs(initial_x_vel) > radius * .8 || abs(initial_y_vel) > radius *
  0.8) {
    throw std::invalid_argument("Please make sure the magnitude of the initial "
                                "velocity of the particles is at most half "
                                "the radius! Tunneling will occur otherwise!");

    // Checks if the particle's initial position is in the container or not
  } else if (x_coord < kXLowerBound || x_coord > kXUpperBound) {
    throw std::invalid_argument("Please make sure the spawn x coordinate is "
                                "within the container boundaries of " +
        std::to_string(kXLowerBound) + " and " + std::to_string
        (kXUpperBound));

  } else if (y_coord < kYLowerBound || y_coord > kYUpperBound) {
    throw std::invalid_argument("Please make sure the spawn y coordinate is "
                                "within the container boundaries of " + 
                                std::to_string(kYLowerBound) + 
                                " and " + std::to_string
                                (kYUpperBound));
  }
}

void ParticleSimulator::Draw() {
  
  // Draws the inner container for the pixels 
  glm::vec2 top_left = glm::vec2(kXLowerBound, kYLowerBound);
  glm::vec2 bottom_right = glm::vec2(kXUpperBound, kYUpperBound);
  ci::Rectf container(top_left, bottom_right);

  ci::gl::color(ci::Color("white"));
  ci::gl::drawStrokedRect(container);
  
  for (Particle& particle : particles_) {
    particle.DrawParticle();
  }
}

void ParticleSimulator::SpeedUp() {
  for (Particle& particle : particles_) {
    particle.SpeedUp();
  }
}

void ParticleSimulator::SlowDown() {
  for (Particle& particle : particles_) {
    particle.SlowDown();
  }
}

const std::vector<Particle> &ParticleSimulator::GetParticles() const {
  return particles_;
}

} // namespace idealgas
