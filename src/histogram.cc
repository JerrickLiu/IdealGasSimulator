#include "histogram.h"

namespace idealgas {

Histogram::Histogram(double mass) {
  bins_ = std::vector<size_t>(kNumberOfPartitions);
  mass_ = mass;
}

void Histogram::Draw(size_t position, const std::vector<Particle> &particles) {

  // We first get the updated particles of a given mass
  particles_ = FindAllParticlesWithMass(particles);
  FillBins(particles_);

  // We get the coordinates to draw out the lines for our histogram
  vertical_axis_top_left_ = glm::vec2(kXLowerBound, position - kHeight);
  vertical_axis_bottom_left_ = glm::vec2(kXLowerBound, position);
  horizontal_axis_bottom_left_ = glm::vec2(kXLowerBound, position);
  horizontal_axis_bottom_right_ = glm::vec2(kXUpperBound, position);
  
  // Set the lower bound for later use
  lower_bound_ = horizontal_axis_bottom_right_.y;

  // Draws the histogram lines
  ci::gl::color(ci::Color("white"));
  ci::gl::drawLine(vertical_axis_top_left_, vertical_axis_bottom_left_);
  ci::gl::drawLine(horizontal_axis_bottom_left_, horizontal_axis_bottom_right_);

  // Draw the title and axis/labels for the histogram
  DrawTitle();
  DrawAxisTitles();
  DrawXAxisLabels();
  DrawYAxisLabels();
  
  // Update the histogram which continuously draws rectangles 
  UpdateHistogram();
}

std::vector<Particle> Histogram::FindAllParticlesWithMass(const std::vector<
    Particle> &particles) const {
  std::vector<Particle> all_particles;
  // Iterates through the list and checks if the particle mass in each of the
  // particles is equal to the desired mass. If so, we copy it into another 
  // vector via the back_inserter parameter with copy_if
  std::copy_if(particles.begin(),
               particles.end(),
               std::back_inserter(all_particles),
               [&](const Particle &particle) {
                 return particle.GetMass() == mass_;
               });
  return all_particles;
}

void Histogram::DrawTitle() {


  // We know mass and color are the same since we sorted them initially so we
  // just get the first particle's mass and color to use in the graph's title
  // size_t mass = particles_.at(0).GetMass();
  std::string color = particles_.at(0).GetColor();

  ci::gl::drawStringCentered("Histogram of " + std::to_string((size_t) mass_) +
                                 " mass (" + color + ") particles",
                             glm::vec2((kXUpperBound - kXLowerBound) * .60,
                                       (lower_bound_ - kHeight
                                           - 20)), ci::Color("white"),
                             ci::Font("Arial", 15));
}

void Histogram::DrawXAxisLabels() {

  double speed_range = kMaxSpeed / kNumberOfPartitions;
  double bar_width = (kXUpperBound - kXLowerBound) / kNumberOfPartitions;
  ci::gl::color(ci::Color("white"));

  // A double for loop that loops through each bar to get the location to 
  // draw the label and the speed that is what actually gets drawn
  for (double bar = 0, speed = 0; bar < kXUpperBound && speed <=
      kMaxSpeed; bar += bar_width, speed += speed_range) {
    
    ci::gl::drawStringCentered(std::to_string((size_t) speed), glm::vec2
                                   (kXLowerBound + bar, (lower_bound_ + 5)),
                               ci::Color("white"),
                               ci::Font("Arial", 15));
  }
}

void Histogram::DrawYAxisLabels() {
  size_t particle_num_range = particles_.size() / kNumberOfPartitions;
  size_t partitions = kHeight / kNumberOfPartitions;
  
  // Two variable for loop that loops through particle ranges that represent 
  // the label on the y axis and the partitions that will be used to locate 
  // where to draw the labels 
  for (size_t num = particle_num_range, height = partitions;
       num < particles_.size() &&
           height < kHeight; num += particle_num_range, height += partitions) {

    ci::gl::drawStringCentered(std::to_string((size_t) num), glm::vec2
                                   (kXLowerBound - 15, (lower_bound_ -
                                       height)),
                               ci::Color("white"),
                               ci::Font("Arial", 15));
  }

  // Labels the max number of particles
  ci::gl::drawStringCentered(std::to_string(particles_.size()), glm::vec2
                                 (kXLowerBound - 15, (lower_bound_ - kHeight)),
                             ci::Color("white"),
                             ci::Font("Arial", 15));
}

void Histogram::DrawAxisTitles() {

  ci::gl::drawStringCentered("Speed",
                             glm::vec2(kXUpperBound / 2, lower_bound_ + 25),
                             ci::Color("white"),
                             ci::Font("Arial", 15));

  ci::gl::drawStringCentered("# of Particles", glm::vec2
                                 (kXLowerBound - 3, (lower_bound_ - kHeight -
                                     20)),
                             ci::Color("white"),
                             ci::Font("Arial", 15));
}


void Histogram::FillBins(const std::vector<Particle> &particles) {

  double speed_range = kMaxSpeed / kNumberOfPartitions;

  for (double min_speed = 0, bin = 0;
       min_speed < kMaxSpeed && bin < kNumberOfPartitions;
       min_speed += speed_range, bin++) {

    // Gets all the particles in the range that will represent the height of 
    // the bar 
    size_t all_particles_in_range = FindAllParticlesInSpeedRange(particles,
                                                                 min_speed,
                                                                 min_speed + speed_range);

    // Populate the bins with the number of particles in each range of speeds
    bins_[bin] = all_particles_in_range;
  }
}


size_t Histogram::FindAllParticlesInSpeedRange(const std::vector<Particle>
                                               &particles,
                                               double min_speed,
                                               double max_speed) const {
  size_t count = 0;
  for (const Particle &particle : particles) {
    double p_speed = glm::length(particle.GetVelocity());

    // We make the lower bound exclusive and the upper bound inclusive to 
    // prevent double counting on the bounds
    if (p_speed >= min_speed && p_speed < max_speed) {
      count++;
    }
  }
  return count;
}

void Histogram::UpdateHistogram() {

  // We first get the bar width and the speed range for our histogram for 
  // each partition in the histogram
  double bar_width = (kXUpperBound - kXLowerBound) / kNumberOfPartitions;
  ci::gl::color(ci::Color("white"));

  // We get the lower bound for the histogram
  double lower_bound = horizontal_axis_bottom_right_.y;

  // A for loop that loops through the bar and the bins. Looping through 
  // each bar allows us to be able to draw the individual bars for each 
  // partition in the histogram. Looping through the bins allows us to get 
  // the number of particles in each bin to be used to draw the rectangles 
  // for the histogram 
  for (double bar = 0, bin = 0; bar < kXUpperBound && bin <
      kNumberOfPartitions; bar += bar_width, bin++) {


    // The scaling factor is used to scale the bars to make it better 
    // visually. This also prevents the bars from going past the height of 
    // the histogram if there are many particles 
    double scaling_factor = 180 / (double) particles_.size();

    // Gets the top left coordinate of the bar. The height is calculated from
    // the number of particles in the range times a scaling factor for 
    // visually look better
    glm::vec2 top_left = glm::vec2(kXLowerBound + bar, lower_bound -
        (bins_[bin] * scaling_factor));
    glm::vec2 bottom_right = glm::vec2(kXLowerBound + bar + bar_width,
                                       lower_bound);
    ci::Rectf h_bar(top_left, bottom_right);

    ci::gl::drawSolidRect(h_bar);
  }
}

const std::vector<size_t> &Histogram::GetBins() const {
  return bins_;
}

} // namespace idealgas
