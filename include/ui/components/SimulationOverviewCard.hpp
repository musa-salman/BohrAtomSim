#ifndef UI_COMPONENTS_SIMULATION_OVERVIEW_CARD_HPP
#define UI_COMPONENTS_SIMULATION_OVERVIEW_CARD_HPP

#include "simulation/model/Simulation.hpp"

namespace ui::components {

class SimulationOverviewCard {
  public:
    static void render(const simulation::model::Simulation &sim);
};

} // namespace ui::components

#endif // UI_COMPONENTS_SIMULATION_OVERVIEW_CARD_HPP
