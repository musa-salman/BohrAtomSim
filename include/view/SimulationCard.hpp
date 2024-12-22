#ifndef SIMULATION_CARD_HPP
#define SIMULATION_CARD_HPP

#include <functional>

#include "Component.hpp"
#include "Simulation.hpp"

class SimulationCard : public Component {
  public:
    explicit SimulationCard(
        const std::function<void(const Simulation *)> &on_simulation_run);

    void draw() override;

    void setSimulation(Simulation &simulation);

  private:
    const std::function<void(const Simulation *)> &on_simulation_run;

    bool is_simulation_running = false;
    Simulation *simulation = nullptr;
};

#endif // SIMULATION_CARD_HPP