#ifndef SIMULATION_MANAGER_VIEW_HPP
#define SIMULATION_MANAGER_VIEW_HPP

#include <functional>
#include <imgui.h>
#include <memory>
#include <string>
#include <unordered_map>

#include "Component.hpp"
#include "Simulation.hpp"
#include "hdf5/SimulationManager.hpp"

class SimulationExplorer : public Component {
  public:
    explicit SimulationExplorer(
        const SimulationManager &simulation_manager,
        const std::function<void(Simulation &)> &on_simulation_selected);

    void draw() override;

  private:
    const SimulationManager &simulation_manager;
    std::function<void(Simulation &)> on_simulation_selected;
    const std::unordered_map<std::string, std::shared_ptr<Simulation>>
        &simulations;
    std::string selected_simulation;
};

#endif // SIMULATION_MANAGER_H
