#ifndef SIMULATION_CARD_HPP
#define SIMULATION_CARD_HPP

#include <functional>

#include "Component.hpp"
#include "simulator_runner/Simulation.hpp"

class SimulationCard : public Component {
  public:
    void draw() override;

    void setOnSimulate(
        const std::function<void(const Simulation *)> &on_simulation_run);

    void setSimulation(Simulation &simulation);

  private:
    std::function<void(const Simulation *)> on_simulation_run;

    bool is_simulation_running = false;
    Simulation *simulation = nullptr;
};

#endif // SIMULATION_CARD_HPP