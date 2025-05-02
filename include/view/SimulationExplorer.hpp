#ifndef SIMULATION_MANAGER_VIEW_HPP
#define SIMULATION_MANAGER_VIEW_HPP

#include <imgui.h>
#include <memory>

#include "Component.hpp"
#include "explorer_manager/OngoingSimulationManager.hpp"
#include "simulator_runner/ISimulator.hpp"
#include "simulator_runner/Simulation.hpp"
#include "view/AddSimulationDialog.hpp"

class SimulationExplorer : public Component {
  public:
    explicit SimulationExplorer();

    void render() override;

  private:
    AddSimulationDialog add_simulation_interface;

    std::shared_ptr<OngoingSimulationManager> simulationManager;
    std::shared_ptr<ISimulator> simulator;

    const std::unordered_map<size_t, std::shared_ptr<Simulation>> &simulations;
    std::unordered_map<std::string, bool> plotSelection;
    Simulation *selected_simulation;
};

#endif // SIMULATION_MANAGER_H
