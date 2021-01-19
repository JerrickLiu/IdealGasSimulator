#include <catch2/catch.hpp>
#include <particle_simulator.h>

using namespace idealgas;

TEST_CASE("Particle controller able to add particles correctly", "[controller]") {
  ParticleSimulator particle_simulator;
  
  SECTION("Adding particles with a radius less than 0.1 throws an error") {
    REQUIRE_THROWS_AS(particle_simulator.AddParticles(50, 0.05, 10, "red"), 
                      std::invalid_argument);
    REQUIRE_THROWS_AS(particle_simulator.AddParticles(50, -5, 10, "red"),
                      std::invalid_argument);
  }
  
  SECTION("Adding particles with no initial velocity throws an error") {
    REQUIRE_THROWS_AS(particle_simulator.AddParticles(5, 10, 10, "red", 
                                                       250, 250, 0, 0), 
            std::invalid_argument);
  }
  
  SECTION("Spawning particles outside the container throws an error") {
    REQUIRE_THROWS_AS(particle_simulator.AddParticles(5, 10, 10, "red", 1, 
                                                       4, 2, 3),
                      std::invalid_argument);
  }
  
  SECTION("Adding particles with a mass less than 0.1 throws an error") {
    REQUIRE_THROWS_AS(particle_simulator.AddParticles(5, 10, 0.05, "red", 1,
                                                       4, 2, 3),
                      std::invalid_argument);
  }
  
  SECTION("Adding 5 particles actually adds 5 particles") {
    particle_simulator.AddParticles(5, 5, 10, "red");
    REQUIRE(particle_simulator.GetParticles().size() == 5);

    SECTION("Update() updates all 5 particles") {
      std::vector<Particle> orig_particles;
      for (const Particle& orig_particle : particle_simulator.GetParticles()) {
        orig_particles.push_back(orig_particle);
      }

      particle_simulator.Update();
      std::vector<Particle> expected_particle;
      bool correct = true;

      for (const Particle& updated_particle : particle_simulator.GetParticles()) {
        expected_particle.push_back(updated_particle);
      }

      for (size_t i = 0; i < expected_particle.size(); i++) {
        if (expected_particle[i].GetPosition() != orig_particles[i].GetPosition() +
            orig_particles[i].GetVelocity()) {
          correct = false;
        }
      }
      REQUIRE(correct);
    }
  }
}

TEST_CASE("Collision with particles", "[collide]") {
  
  SECTION("There shouldn't be a collision if particles aren't touching") {
    ParticleSimulator particle_simulator;
    particle_simulator.AddParticles(1, 10, 10, "red", 550,
                                    250, -2, 0);
    particle_simulator.AddParticles(1, 10, 10, "red", 540,
                                    550, 2, 0);
    particle_simulator.Update();

    std::vector<Particle> particles = particle_simulator.GetParticles();
    
    SECTION("Velocity should be the same") {
      REQUIRE(particles[0].GetVelocity() == glm::vec2(-2, 0));
      REQUIRE(particles[1].GetVelocity() == glm::vec2(2, 0));
    }
    
    SECTION("Position is updated correctly") {
      REQUIRE(particles[0].GetPosition() == glm::vec2(548, 250));
      REQUIRE(particles[1].GetPosition() == glm::vec2(542, 550));
    }
  }
  
  SECTION("Particles moving toward each other with same mass should collide") {
    ParticleSimulator particle_simulator;
    particle_simulator.AddParticles(1, 10, 10, "red", 550,
                                     550, -2, 0);
    particle_simulator.AddParticles(1, 10, 10, "red", 540,
                                     550, 2, 0);
    particle_simulator.Update();

    std::vector<Particle> particles = particle_simulator.GetParticles();

    SECTION("Distance should be less than sum of radii") {
      REQUIRE(glm::distance(particles[0].GetPosition(), particles[1].GetPosition()) < 20);
    }

    SECTION("New velocity of both particles after collision is correct") {

      // Since they only have a horizontal velocity, once they collide, they 
      // should just bounce in the opposite direction so their 
      // horizontal velocities would just be negated
      REQUIRE(particles[0].GetVelocity() == glm::vec2(2, 0));
      REQUIRE(particles[1].GetVelocity() == glm::vec2(-2, 0));

      SECTION("Velocity is conserved") {
        double updated_total_vel = particles[0].GetVelocity().x +
            particles[1].GetVelocity().x;
        REQUIRE(updated_total_vel == 0);
      }

      SECTION("Position is updated correctly") {
        REQUIRE(particles[0].GetPosition() == glm::vec2(552, 550));
        REQUIRE(particles[1].GetPosition() == glm::vec2(538, 550));
      }

      SECTION("Particles should not stick together after collision") {
        particle_simulator.Update();

        // If the particles collided again, then their velocities would have 
        // negated again after the update. This checks that their velocities 
        // stayed the same after an update to ensure they didn't collide 
        // again and get stuck in a loop
        REQUIRE(particles[0].GetVelocity() == glm::vec2(2, 0));
        REQUIRE(particles[1].GetVelocity() == glm::vec2(-2, 0));
      }
    }
  }

  SECTION("Collision with particles with same mass that have both X and Y "
          "velocity") {
    ParticleSimulator particle_simulator;
    particle_simulator.AddParticles(1, 10, 10, "red", 550,
                                     547, -2, 3);
    particle_simulator.AddParticles(1, 10, 10, "red", 540,
                                     545, 2, 5);

    particle_simulator.Update();
    std::vector<Particle> particles = particle_simulator.GetParticles();
    double initial_tot_vel = 8;

    SECTION("Distance should be less than sum of radii") {
      REQUIRE(glm::distance(particles[0].GetPosition(), particles[1].GetPosition()) < 20);
    }

    SECTION("New velocity of both particles after collision is correct") {
      Approx p1_new_x_velocity = Approx(2.23).epsilon(0.1);
      Approx p1_new_y_velocity = Approx(3.85).epsilon(0.1);
      REQUIRE(particles[0].GetVelocity().x == p1_new_x_velocity);
      REQUIRE(particles[0].GetVelocity().y == p1_new_y_velocity);

      Approx p2_new_x_velocity = Approx(-2.23).epsilon(0.1);
      Approx p2_new_y_velocity = Approx(4.15).epsilon(0.1);
      REQUIRE(particles[1].GetVelocity().x == p2_new_x_velocity);
      REQUIRE(particles[1].GetVelocity().y == p2_new_y_velocity);

      SECTION("Velocity between the collision should be conserved") {
        double updated_total_vel = 0.0;
        for (const Particle& particle : particles) {
          updated_total_vel += particle.GetVelocity().x;
          updated_total_vel += particle.GetVelocity().y;
        }
        REQUIRE(updated_total_vel == initial_tot_vel);
      }

      SECTION("Position is updated correctly") {
        Approx p1_x = Approx(552.2).epsilon(0.1);
        Approx p1_y = Approx(550.8).epsilon(0.1);

        REQUIRE(particles[0].GetPosition().x == p1_x);
        REQUIRE(particles[0].GetPosition().y == p1_y);

        Approx p2_x = Approx(537.8).epsilon(0.1);
        Approx p2_y = Approx(549.2).epsilon(0.1);
        REQUIRE(particles[1].GetPosition().x == p2_x);
        REQUIRE(particles[1].GetPosition().y == p2_y);
      }

      SECTION("Particles should not stick together after collision") {
        particle_simulator.Update();

        // Velocities should stay the same after update (no collision occurred)
        REQUIRE(particles[0].GetVelocity().x == p1_new_x_velocity);
        REQUIRE(particles[0].GetVelocity().y == p1_new_y_velocity);
        REQUIRE(particles[1].GetVelocity().x == p2_new_x_velocity);
        REQUIRE(particles[1].GetVelocity().y == p2_new_y_velocity);
      }
    }
  }
  SECTION("Collision of particles with different mass") {
    SECTION("Particles with no vertical velocity component") {
      ParticleSimulator particle_simulator;
      particle_simulator.AddParticles(1, 10, 10, "red", 550,
                                       545, -3, 0);
      particle_simulator.AddParticles(1, 20, 20, "red", 540,
                                       545, 3, 0);

      particle_simulator.Update();

      std::vector<Particle> particles = particle_simulator.GetParticles();
      
      double initial_tot_KE = 0.5 * 10 * pow(-3, 2) + 0.5 * 20 * pow(3, 2);

      SECTION("Distance should be less than sum of radii") {
        REQUIRE(glm::distance(particles[0].GetPosition(), particles[1].GetPosition()) < 30);
      }

      SECTION("New velocity of both particles after collision is correct") {

        // Since they only have a horizontal velocity, once they collide, they 
        // should just bounce in the opposite direction so their 
        // horizontal velocities would just be negated
        REQUIRE(particles[0].GetVelocity() == glm::vec2(5, 0));
        REQUIRE(particles[1].GetVelocity() == glm::vec2(-1, 0));

        SECTION("Kinetic energy is conserved") {
          double updated_total_KE = 0.5 * 10 * pow(particles[0].GetVelocity().x, 2) 
              + 0.5 * 20 * pow(particles[1].GetVelocity().x, 2);
          REQUIRE(updated_total_KE == initial_tot_KE);
        }

        SECTION("Position is updated correctly") {
          REQUIRE(particles[0].GetPosition() == glm::vec2(555, 545));
          REQUIRE(particles[1].GetPosition() == glm::vec2(539, 545));
        }

        SECTION("Particles should not stick together after collision") {
          particle_simulator.Update();

          // If the particles collided again, then their velocities would have 
          // negated again after the update. This checks that their velocities 
          // stayed the same after an update to ensure they didn't collide 
          // again and get stuck in a loop
          REQUIRE(particles[0].GetVelocity() == glm::vec2(5, 0));
          REQUIRE(particles[1].GetVelocity() == glm::vec2(-1, 0));
        }
      }
    }

    SECTION("Particles with both velocity component") {
      ParticleSimulator particle_simulator;
      particle_simulator.AddParticles(1, 10, 10, "red", 550,
                                      545, -3, 5);
      particle_simulator.AddParticles(1, 20, 20, "red", 540,
                                      545, 3, 3);

      particle_simulator.Update();

      std::vector<Particle> particles = particle_simulator.GetParticles();

      SECTION("Distance should be less than sum of radii") {
        REQUIRE(glm::distance(particles[0].GetPosition(), particles[1].GetPosition()) < 30);
      }

      SECTION("New velocity of both particles after collision is correct") {
        REQUIRE(particles[0].GetVelocity() == glm::vec2(5, 5));
        REQUIRE(particles[1].GetVelocity() == glm::vec2(-1, 3));

        SECTION("Position is updated correctly") {
          REQUIRE(particles[0].GetPosition() == glm::vec2(555, 550));
          REQUIRE(particles[1].GetPosition() == glm::vec2(539, 548));
        }

        SECTION("Particles should not stick together after collision") {
          particle_simulator.Update();

          // If the particles collided again, then their velocities would have 
          // negated again after the update. This checks that their velocities 
          // stayed the same after an update to ensure they didn't collide 
          // again and get stuck in a loop
          REQUIRE(particles[0].GetVelocity() == glm::vec2(5, 5));
          REQUIRE(particles[1].GetVelocity() == glm::vec2(-1, 3));
        }
      }
    }
  }
  
  SECTION("Collisions can be detected between pairs of particles with more "
          "than two particles") {
    ParticleSimulator particle_simulator;
    particle_simulator.AddParticles(1, 10, 10, "red", 550,
                                    545, -3, 5);
    particle_simulator.AddParticles(1, 20, 20, "red", 540,
                                    545, 3, 3);

    particle_simulator.AddParticles(1, 20, 20, "red", 545,
                                    550, 3, -4);

    particle_simulator.Update();

    std::vector<Particle> particles = particle_simulator.GetParticles();

    SECTION("Distance should be less than sum of radii") {
      REQUIRE(glm::distance(particles[0].GetPosition(), particles[1].GetPosition()) < 30);
      REQUIRE(glm::distance(particles[0].GetPosition(), particles[2]
      .GetPosition()) < 30);
      REQUIRE(glm::distance(particles[1].GetPosition(), particles[2]
          .GetPosition()) < 40);
    }

    SECTION("New velocity of all particles after collision is correct") {
      Approx p1_v_x = Approx(9.7).epsilon(0.1);
      Approx p1_v_y = Approx(0.33).epsilon(0.1);

      REQUIRE(particles[0].GetVelocity().x == p1_v_x);
      REQUIRE(particles[0].GetVelocity().y == p1_v_y);

      REQUIRE(particles[1].GetVelocity() == glm::vec2(-2.5, 1.5));

      Approx p3_v_x = Approx(2.2).epsilon(0.1);
      Approx p3_v_y = Approx(-0.17).epsilon(0.1);

      REQUIRE(particles[2].GetVelocity().x == p3_v_x);
      REQUIRE(particles[2].GetVelocity().y == p3_v_y);

      SECTION("Position is updated correctly") {
        Approx p1_x = Approx(559.7).epsilon(0.1);
        Approx p1_y = Approx(545.3).epsilon(0.1);
        
        REQUIRE(particles[0].GetPosition().x == p1_x);
        REQUIRE(particles[0].GetPosition().y == p1_y);

        Approx p2_x = Approx(537.5).epsilon(0.1);
        Approx p2_y = Approx(546.5).epsilon(0.1);

        REQUIRE(particles[1].GetPosition().x == p2_x);
        REQUIRE(particles[1].GetPosition().y == p2_y);

        Approx p3_x = Approx(547.2).epsilon(0.1);
        Approx p3_y = Approx(549.8).epsilon(0.1);

        REQUIRE(particles[2].GetPosition().x == p3_x);
        REQUIRE(particles[2].GetPosition().y == p3_y);
      }

      SECTION("Particles should not stick together after collision") {
        particle_simulator.Update();

        // If the particles collided again, then their velocities would have 
        // changed again after the update. This checks that their velocities 
        // stayed the same after an update to ensure they didn't collide 
        // again and get stuck in a loop

        REQUIRE(particles[0].GetVelocity().x == p1_v_x);
        REQUIRE(particles[0].GetVelocity().y == p1_v_y);
        REQUIRE(particles[1].GetVelocity() == glm::vec2(-2.5, 1.5));
        REQUIRE(particles[2].GetVelocity().x == p3_v_x);
        REQUIRE(particles[2].GetVelocity().y == p3_v_y);
      }
    }
  }
}

TEST_CASE("Velocity is conserved throughout the simulation with particles of "
          "the same mass", "[conserve]") {
  ParticleSimulator particle_simulator;
  particle_simulator.AddParticles(50, 5, 10, "red");
  double initial_tot_vel = 0.0;
  for (const Particle& particle : particle_simulator.GetParticles()) {
    initial_tot_vel += abs(particle.GetVelocity().x);
    initial_tot_vel += abs(particle.GetVelocity().y);
  }
  
  for (size_t i = 0; i < 100; i++) {
    particle_simulator.Update();
  }

  double new_tot_vel = 0.0;
  for (const Particle& particle : particle_simulator.GetParticles()) {
    new_tot_vel += abs(particle.GetVelocity().x);
    new_tot_vel += abs(particle.GetVelocity().y);
  }
  
  // We use approx because of doubles and rounding while updating 
  REQUIRE(new_tot_vel == Approx(initial_tot_vel).epsilon(1));
}

TEST_CASE("Kinetic energy is conserved throughout the simulation with "
          "particles of the different mass", "[conserve]") {
  ParticleSimulator particle_simulator;
  particle_simulator.AddParticles(50, 5, 10, "red");
  particle_simulator.AddParticles(50, 10, 20, "red");
  double initial_tot_KE = 0.0;
  for (const Particle& particle : particle_simulator.GetParticles()) {
    double total_vel = 0.0;
    total_vel += abs(particle.GetVelocity().x);
    total_vel += abs(particle.GetVelocity().y);
    initial_tot_KE += 0.5 * particle.GetMass() * pow(total_vel, 2);
  }

  for (size_t i = 0; i < 100; i++) {
    particle_simulator.Update();
  }

  double new_tot_KE = 0.0;
  for (const Particle& particle : particle_simulator.GetParticles()) {
    double total_vel = 0.0;
    total_vel += abs(particle.GetVelocity().x);
    total_vel += abs(particle.GetVelocity().y);
    new_tot_KE += 0.5 * particle.GetMass() * pow(total_vel, 2);
  }

  // We use approx because of doubles and rounding while updating 
  REQUIRE(initial_tot_KE == Approx(new_tot_KE).epsilon(1));
}
