#include <particle.h>
#include <particle_simulator.h>

namespace idealgas {

Particle::Particle(const glm::vec2& location, const glm::vec2& velocity,
                   double radius, double mass, const std::string& color) {
  velocity_ = velocity;
  position_ = location;
  radius_ = radius;
  mass_ = mass;
  color_ = color;
}

void Particle::DrawParticle() {
  ci::gl::color(ci::Color(color_.c_str()));
  ci::gl::drawSolidCircle(position_, radius_);
}
void Particle::Update() {
  position_ += velocity_;

  // These checks prevent the particle from getting stuck on the wall. Ex if 
  // the horizontal velocity of the particle is negative (moving to the left)
  // and it is on the right side of the left vertical wall, then we know it's
  // moving toward it
  if (velocity_.x < 0) {
    if (position_.x <= ParticleSimulator::kXLowerBound + radius_) {
      velocity_.x = -velocity_.x;
    }
  }

  if (velocity_.y < 0) {
    if (position_.y <= ParticleSimulator::kYLowerBound + radius_) {
      velocity_.y = -velocity_.y;
    }
  }
  
  if (velocity_.x > 0) {
    if (position_.x >= ParticleSimulator::kXUpperBound - radius_) {
      velocity_.x = -velocity_.x;
    }
  }

  if (velocity_.y > 0) {
    if (position_.y >= ParticleSimulator::kYUpperBound - radius_) {
      velocity_.y = -velocity_.y;
    }
  }
}

void Particle::SpeedUp() {
  
  // Checks to make sure the magnitude of the velocity components are less 
  // than half of the particle's radius to prevent tunneling
  if (velocity_.x < radius_ / 2 && velocity_.x > 0) {
    velocity_.x += 0.5;
    
  } else if (velocity_.x > - radius_ / 2 && velocity_.x < 0) {
    velocity_.x -= 0.5;
  }
  
  if (velocity_.y < radius_ / 2 && velocity_.x > 0) {
    velocity_.y += 0.5;
    
  } else if (velocity_.y > - radius_ / 2 && velocity_.x < 0) {
    velocity_.y -= 0.5;
  }
}

void Particle::SlowDown() {
  
  // Checks to make sure the magnitude of the velocity components are less 
  // than 0
  if (velocity_.x > 0) {
    velocity_.x -= 0.5;
    
  } else if (velocity_.x < 0) {
    velocity_.x += 0.5;
  }
  
  if (velocity_.y > 0) {
    velocity_.y -= 0.5;
    
  } else if (velocity_.y < 0) {
    velocity_.y += 0.5;
  }
}

const glm::vec2 &Particle::GetPosition() const {
  return position_;
}
const glm::vec2 &Particle::GetVelocity() const {
  return velocity_;
}
void Particle::SetVelocity(const glm::vec2 &velocity) {
  velocity_ = velocity;
}
double Particle::GetRadius() const {
  return radius_;
}
double Particle::GetMass() const {
  return mass_;
}
const std::string &Particle::GetColor() const {
  return color_;
}

} // namespace idealgas
