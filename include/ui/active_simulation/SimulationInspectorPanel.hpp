#ifndef UI_ACTIVE_SIMULATION_SIMULATION_INSPECTOR_PANEL_HPP
#define UI_ACTIVE_SIMULATION_SIMULATION_INSPECTOR_PANEL_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "simulator_runner/Simulation.hpp"

namespace ui::active_simulation {

class SimulationInspectorPanel {
  public:
    void setSimulation(const std::shared_ptr<Simulation> &sim);
    void render();

  private:
    std::weak_ptr<Simulation> simulation;
    std::unordered_map<std::string, bool> plotSelection;
};

} // namespace ui::active_simulation

#endif // UI_ACTIVE_SIMULATION_SIMULATION_INSPECTOR_PANEL_HPP
