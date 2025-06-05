#ifndef UI_ACTIVE_SIMULATION_ACTIVE_SIMULATION_PANEL_HPP
#define UI_ACTIVE_SIMULATION_ACTIVE_SIMULATION_PANEL_HPP

#include "AddSimulationDialog.hpp"
#include "SimulationInspectorPanel.hpp"
#include "SimulationListPanel.hpp"
#include "simulation_repositories/SimulationService.hpp"

namespace ui::active_simulation {

class ActiveSimulationPanel {
  public:
    ActiveSimulationPanel();

    void render();

  private:
    size_t selectedSimulationId = 0;

    AddSimulationDialog addSimulationWindow;
    SimulationListPanel listPanel;
    SimulationInspectorPanel inspectorPanel;

    SimulationService &simulationService;
};

} // namespace ui::active_simulation

#endif // UI_ACTIVE_SIMULATION_ACTIVE_SIMULATION_PANEL_HPP
