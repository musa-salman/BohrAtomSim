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

    void draw() override;

  private:
    std::function<void(const Simulation &)> on_submit;

    Simulation simulation;
};

#endif