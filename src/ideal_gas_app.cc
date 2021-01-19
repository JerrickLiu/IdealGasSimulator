#include <ideal_gas_app.h>


namespace idealgas {


IdealGasApp::IdealGasApp() {
  ci::app::setWindowSize(ParticleSimulator::kWindowSizeWidth, ParticleSimulator::kWindowSizeHeight);
}


void IdealGasApp::setup() {
  particle_simulator_.AddParticles(kSmallParticleAmount,
                                   kSmallParticleRadius, kSmallParticleMass, kSmallParticleColor);
  histograms_.push_back(Histogram(kSmallParticleMass));

  particle_simulator_.AddParticles(kMediumParticleAmount,
                                   kMediumParticleRadius, kMediumParticleMass, kMediumParticleColor);
  histograms_.push_back(Histogram(kMediumParticleMass));

  particle_simulator_.AddParticles(kBigParticleAmount, kBigParticleRadius,
                                   kBigParticleMass,kBigParticleColor);
  histograms_.push_back(Histogram(kBigParticleMass));
}

void IdealGasApp::draw() {
  ci::Color8u background_color(0, 0, 0);  // black
  ci::gl::clear(background_color);
  particle_simulator_.Draw();

  size_t num_histograms = histograms_.size();
  size_t index = 0;
  
  for (Histogram& histogram : histograms_) {
    index++;

    // We then draw the histogram based on the number of histograms there 
    // are. The equation used to find the position allows for it to scale 
    // based on the different number of particle masses
    histogram.Draw((ParticleSimulator::kYUpperBound / num_histograms) *
        1.05 * index, particle_simulator_.GetParticles());
  }

  ci::gl::drawStringCentered(
      "Press the left arrow to slow down the simulation. Press the right "
      "arrow to speed up the simulation!",
      glm::vec2(ParticleSimulator::kWindowSizeWidth * .60, ParticleSimulator::kYLowerBound / 2),
      ci::Color("white"), ci::Font("Times New Roman", 20));
}

void IdealGasApp::update() {
  particle_simulator_.Update();
}

void IdealGasApp::keyDown(ci::app::KeyEvent event) {
  switch (event.getCode()) {
    case ci::app::KeyEvent::KEY_LEFT:
      particle_simulator_.SlowDown();
      break;
      
    case ci::app::KeyEvent::KEY_RIGHT:
      particle_simulator_.SpeedUp();
      break;
  } 
}

}  // namespace naivebayes
