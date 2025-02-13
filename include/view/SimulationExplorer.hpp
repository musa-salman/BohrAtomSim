#ifndef SIMULATION_MANAGER_VIEW_HPP
#define SIMULATION_MANAGER_VIEW_HPP

#include <imgui.h>

#include "Component.hpp"
#include "explorer_manager/OngoingSimulationManager.hpp"
#include "simulator_runner/Simulation.hpp"
#include "simulator_runner/Simulator.hpp"
#include "view/AddSimulationDialog.hpp"
#include "view/SimulationCard.hpp"

class SimulationExplorer : public Component {
  public:
    explicit SimulationExplorer(OngoingSimulationManager &simulation_manager,
                                Simulator &simulator);

    void draw() override;

  private:
    AddSimulationDialog add_simulation_interface;
    SimulationCard simulation_card;
    OngoingSimulationManager &simulation_manager;
    Simulator &simulator;
    const std::unordered_map<size_t, std::shared_ptr<Simulation>> &simulations;
    std::unordered_map<std::string, bool> plot_selection;
    size_t selected_simulation;
};

#endif // SIMULATION_MANAGER_H
