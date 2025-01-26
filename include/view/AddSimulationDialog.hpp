#ifndef ADD_SIMULATION_DIALOG_HPP
#define ADD_SIMULATION_DIALOG_HPP

#include <functional>

#include "Component.hpp"
#include "Simulation.hpp"
#include "atom/atom_bohr_sim.h"
#include "hdf5/SimulationFields.hpp"

class AddSimulationDialog : public Component {
  public:
    explicit AddSimulationDialog(
        const std::function<void(const Simulation &)> &on_submit);

    void draw() override;

  private:
    const std::function<void(const Simulation &)> on_submit;

    Simulation simulation = Simulation("", POLAR, SIMULATION_2D_NON_REL);
};

#endif