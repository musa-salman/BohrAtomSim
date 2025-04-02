#ifndef ADD_SIMULATION_DIALOG_HPP
#define ADD_SIMULATION_DIALOG_HPP

#include <functional>

#include "Component.hpp"
#include "atom/atom_bohr_sim.h"
#include "simulator_runner/Simulation.hpp"

class AddSimulationDialog : public Component {
  public:
    explicit AddSimulationDialog();

    void setOnSubmit(const std::function<void(const Simulation &)> &on_submit);

    void render() override;

  private:
    std::function<void(const Simulation &)> on_submit;
    char date[20];
    Simulation simulation;

    void resetSimulation();

    void formatName(char *name);
};

#endif