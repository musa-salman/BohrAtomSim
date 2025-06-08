#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "simulator_runner/Simulation.hpp"

namespace ui::active_simulation::components {

class SimulationInspectorPanel {
  public:
    void setSimulation(const std::shared_ptr<Simulation> &sim);
    void render();

  private:
    std::weak_ptr<Simulation> simulation;
    std::unordered_map<std::string, bool> plotSelection;
};

} // namespace ui::active_simulation::components
