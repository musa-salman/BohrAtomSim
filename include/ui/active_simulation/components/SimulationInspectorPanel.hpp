#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "simulation/model/Simulation.hpp"
#include "ui/components/SimulationEditorDialog.hpp"

namespace ui::active_simulation::components {

class SimulationInspectorPanel {
  public:
    SimulationInspectorPanel();
    void
    setSimulation(const std::shared_ptr<simulation::model::Simulation> &sim);
    void render();

  private:
    std::weak_ptr<simulation::model::Simulation> m_simulation;
    std::unordered_map<std::string, bool> m_plotSelection;
    ui::components::SimulationEditorDialog m_simulationEditorDialog;
};

} // namespace ui::active_simulation::components
