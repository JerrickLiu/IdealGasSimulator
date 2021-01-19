#include <catch2/catch.hpp>
#include <particle_simulator.h>
#include <histogram.h>

using namespace idealgas;

TEST_CASE("Particles fall into the correct speed ranges") {
  ParticleSimulator particle_simulator;
  particle_simulator.AddParticles(30, 50, 10, "red",
                                  500, 600, 25, 0);
  particle_simulator.AddParticles(10, 50, 10, "red",
                                  900, 700, 6, 0);
  particle_simulator.AddParticles(20, 50, 10, "red",
                                  600, 500, 17, 0);

  particle_simulator.AddParticles(5, 50, 30, "blue",
                                  500, 250, 25, 0);
  particle_simulator.AddParticles(90, 50, 30, "blue",
                                  900, 350, 5, 0);
  particle_simulator.AddParticles(25, 50, 30, "blue",
                                  600, 90, 15, 0);
  
  Histogram red_histogram(10);
  Histogram blue_histogram(30);
  
  std::vector<Particle> particles = particle_simulator.GetParticles();


  SECTION("Histogram accurately finds all the particles with a given mass") {
    REQUIRE(red_histogram.FindAllParticlesWithMass(particles).size() == 60);
    REQUIRE(blue_histogram.FindAllParticlesWithMass(particles).size() == 120);
  }
  
  SECTION("Red histogram accurately partitions all the particles into its "
          "bins") {

    std::vector<Particle> all_red_particles = red_histogram.FindAllParticlesWithMass(particles);
    
    // The histogram is partitioned 10 ways with a maximum velocity of 50 so 
    // the bins are in groups of speeds of 5. The min of the range is 
    // exclusive and the max of the range is inclusive so from 0-5, 5 would 
    // not be included in the first bin
    red_histogram.FillBins(all_red_particles);
    std::vector<size_t> bins = red_histogram.GetBins();
    
    // Particles with speeds from 0-5 
    REQUIRE(bins[0] == 0);
    
    // Particles with speeds from 5-10
    REQUIRE(bins[1] == 10);

    // And so on...
    REQUIRE(bins[2] == 0);
    
    // There are 20 particles in the range from 15-20
    REQUIRE(bins[3] == 20);
    REQUIRE(bins[4] == 0);
    
    // There are 5 particles in the range from 25-30
    REQUIRE(bins[5] == 30);
    REQUIRE(bins[6] == 0);
    REQUIRE(bins[7] == 0);
    REQUIRE(bins[8] == 0);
    REQUIRE(bins[9] == 0);
  }

  SECTION("Blue histogram accurately partitions all the particles into its "
          "bins") {

    std::vector<Particle> all_blue_particles = blue_histogram
        .FindAllParticlesWithMass(particles);
    
    blue_histogram.FillBins(all_blue_particles);
    std::vector<size_t> bins = blue_histogram.GetBins();

    // Particles with speeds from 0-5 
    REQUIRE(bins[0] == 0);

    // Particles with speeds from 5-10
    REQUIRE(bins[1] == 90);

    // And so on...
    REQUIRE(bins[2] == 0);

    // There are 25 particles in the range from 15-20
    REQUIRE(bins[3] == 25);
    REQUIRE(bins[4] == 0);

    // There are 5 particles in the range from 25-30
    REQUIRE(bins[5] == 5);
    REQUIRE(bins[6] == 0);
    REQUIRE(bins[7] == 0);
    REQUIRE(bins[8] == 0);
    REQUIRE(bins[9] == 0);
  }

  SECTION("Adding particles with the color and mass of the red particles "
          "updates the red histogram but doesn't change the blue "
          "histogram") {
    particle_simulator.AddParticles(20, 50, 10, "red",
                                    600, 90, 16, 0);

    std::vector<Particle> new_particles = particle_simulator
        .GetParticles();

    std::vector<Particle> all_red_particles = red_histogram
        .FindAllParticlesWithMass(new_particles);

    std::vector<Particle> all_blue_particles = blue_histogram
        .FindAllParticlesWithMass(new_particles);
    
    red_histogram.FillBins(all_red_particles);
    blue_histogram.FillBins(all_blue_particles);
    
    SECTION("The 4th bin (speeds from 15-20) should be updated for red "
            "histogram but not for blue") {
      REQUIRE(red_histogram.GetBins()[3] == 40);
      REQUIRE(blue_histogram.GetBins()[3] != 40);
    }
  }
}