#include <catch2/catch.hpp>
#include <particle.h>
#include <particle_simulator.h>

using namespace idealgas;
using glm::vec2;

TEST_CASE("Particle is created correctly") {
  Particle particle(vec2(250, 100), vec2(2, 5), 5, 10, "red");
  
  SECTION("Velocity is stored correctly") {
    REQUIRE(particle.GetVelocity().x == 2);
    REQUIRE(particle.GetVelocity().y == 5);
  }
  
  SECTION("Initial position is stored correctly") {
    REQUIRE(particle.GetPosition().x == 250);
    REQUIRE(particle.GetPosition().y == 100);
  }
  
  SECTION("Radius is stored correctly") {
    REQUIRE(particle.GetRadius() == 5);
  }
  
  SECTION("Mass is stored correctly") {
    REQUIRE(particle.GetMass() == 10);
  }
}

TEST_CASE("Particle's position is updated correctly with Update()") {
  Particle particle(vec2(250, 250), 
                    vec2(5, -2), 5, 10, "red");
  particle.Update();
  REQUIRE(particle.GetPosition() == vec2(255, 248));
}

TEST_CASE("Particle has correct behavior with walls", "[velocity]") {
  SECTION("Horizontal velocity should be negated after hitting right vertical "
          "wall") {
    Particle particle(vec2(ParticleSimulator::kXUpperBound - 5, 25), 
                      vec2(5, 0), 5, 10, "red");
    double curr_x_vel = particle.GetVelocity().x;
    particle.Update();
    double updated_x_vel = particle.GetVelocity().x;
    REQUIRE(curr_x_vel == -updated_x_vel);
    
    SECTION("Position is updated correctly after wall collision") {
      REQUIRE(particle.GetPosition() == vec2(1350, 25));
    }

    SECTION("Horizontal velocity should not be negated if moving in other "
            "direction of wall (particle should not get stuck on wall)") {
      particle.Update();
      double new_x_vel = particle.GetVelocity().x;
      REQUIRE(updated_x_vel != -new_x_vel);
    }
  }

  SECTION("Horizontal velocity should not be negated if not touching right "
          "vertical wall") {
    Particle particle(vec2(ParticleSimulator::kXUpperBound - 25, 25),
                      vec2(5, 0), 5, 10, "red");
    double curr_x_vel = particle.GetVelocity().x;
    particle.Update();
    double updated_x_vel = particle.GetVelocity().x;
    REQUIRE(curr_x_vel != -updated_x_vel);
    
    SECTION("Position is updated correctly") {
      REQUIRE(particle.GetPosition() == vec2(1330, 25));
    }
  }

  SECTION("Horizontal velocity should be negated after hitting left vertical "
          "wall") {
    Particle particle(vec2(ParticleSimulator::kXLowerBound + 5, 25),
                      vec2(-5, 0), 5, 10, "red");
    double curr_x_vel = particle.GetVelocity().x;
    particle.Update();
    double updated_x_vel = particle.GetVelocity().x;
    REQUIRE(curr_x_vel == -updated_x_vel);

    SECTION("Position is updated correctly") {
      REQUIRE(particle.GetPosition() == vec2(450, 25));
    }

    SECTION("Horizontal velocity should not be negated if moving in other "
            "direction of wall (particle should not get stuck on wall)") {
      particle.Update();
      double new_x_vel = particle.GetVelocity().x;
      REQUIRE(updated_x_vel != -new_x_vel);
    }
  }
  
  SECTION("Horizontal velocity should not be negated if not touching left "
          "vertical wall") {
    Particle particle(vec2(ParticleSimulator::kXLowerBound + 25, 25),
                      vec2(-5, 0), 5, 10, "red");
    double curr_x_vel = particle.GetVelocity().x;
    particle.Update();
    double updated_x_vel = particle.GetVelocity().x;
    REQUIRE(curr_x_vel != -updated_x_vel);
  }

  SECTION("Vertical velocity should be negated after hitting lower horizontal "
          "wall") {
    Particle particle(vec2(500, ParticleSimulator::kYUpperBound - 5), 
                      vec2(0, 5), 5, 10, "red");
    double curr_y_vel = particle.GetVelocity().y;
    particle.Update();
    double updated_y_vel = particle.GetVelocity().y;
    REQUIRE(curr_y_vel == -updated_y_vel);

    SECTION("Position is updated correctly") {
      REQUIRE(particle.GetPosition() == vec2(500, 720));
    }

    SECTION("Vertical velocity should not be negated if moving in other "
            "direction of wall (particle should not get stuck on wall)") {
      particle.Update();
      double new_y_vel = particle.GetVelocity().y;
      REQUIRE(updated_y_vel != -new_y_vel);
    }
  }

  SECTION("Vertical velocity should not be negated if not touching lower "
          "horizontal wall") {
    Particle particle(vec2(500, ParticleSimulator::kYUpperBound - 25),
                      vec2(0, 5), 5, 10, "red");
    double curr_y_vel = particle.GetVelocity().y;
    particle.Update();
    double updated_y_vel = particle.GetVelocity().y;
    REQUIRE(curr_y_vel != -updated_y_vel);
  }

  SECTION("Vertical velocity should be negated after hitting top horizontal "
          "wall") {
    Particle particle(vec2(500, ParticleSimulator::kYLowerBound + 5),
                      vec2(0, -5), 5, 10, "red");
    double curr_y_vel = particle.GetVelocity().y;
    particle.Update();
    double updated_y_vel = particle.GetVelocity().y;
    REQUIRE(curr_y_vel == -updated_y_vel);

    SECTION("Position is updated correctly") {
      REQUIRE(particle.GetPosition() == vec2(500, 80));
    }

    SECTION("Vertical velocity should not be negated if moving in other "
            "direction of wall (particle should not get stuck on wall)") {
      particle.Update();
      double new_y_vel = particle.GetVelocity().y;
      REQUIRE(updated_y_vel != -new_y_vel);
    }
  }

  SECTION("Vertical velocity should not be negated if not touching top "
          "horizontal wall") {
    Particle particle(vec2(500, ParticleSimulator::kYLowerBound + 25),
                      vec2(0, -5), 5, 10, "red");
    double curr_y_vel = particle.GetVelocity().y;
    particle.Update();
    double updated_y_vel = particle.GetVelocity().y;
    REQUIRE(curr_y_vel != -updated_y_vel);
  }
  
  SECTION("Both horizontal and vertical velocity should be negated if it hits"
          " the corner of the container") {
    Particle particle(vec2(ParticleSimulator::kXUpperBound - 10,
                           ParticleSimulator::kYUpperBound - 10),vec2(5, 5), 
                      5, 10,"red");
    
    double curr_x_vel = particle.GetVelocity().x;
    double curr_y_vel = particle.GetVelocity().y;
    
    particle.Update();
    
    double updated_x_vel = particle.GetVelocity().x;
    double updated_y_vel = particle.GetVelocity().y;
    
    REQUIRE(curr_x_vel == -updated_x_vel);
    REQUIRE(curr_y_vel == -updated_y_vel);

    SECTION("Position is updated correctly") {
      REQUIRE(particle.GetPosition() == vec2(1345, 715));
    }
  }
}