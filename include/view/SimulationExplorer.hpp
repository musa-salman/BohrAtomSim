#ifndef SIMULATION_MANAGER_VIEW_HPP
#define SIMULATION_MANAGER_VIEW_HPP

#include <imgui.h>

#include "Component.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "view/AddSimulationDialog.hpp"

class SimulationExplorer : public Component {
  public:
    explicit SimulationExplorer();

    void render() override;

  private:
    AddSimulationDialog addSimulationWindow;

    SimulationService &simulationService;

    std::unordered_map<std::string, bool> plotSelection;

    size_t selectedSimulationId;
    Simulation::SimulationStatus *selectedSimulationStatus;
};
#endif // SIMULATION_MANAGER_H
