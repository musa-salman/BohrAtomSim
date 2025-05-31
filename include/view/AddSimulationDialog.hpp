#ifndef ADD_SIMULATION_DIALOG_HPP
#define ADD_SIMULATION_DIALOG_HPP

#include <functional>
#include <vector>

#include "Component.hpp"
#include "simulator_runner/Simulation.hpp"
#include "steppers/Potential.hpp"
#include "steppers/StateBuilder.hpp"

class AddSimulationDialog : public Component {
  public:
    explicit AddSimulationDialog();

    void setOnSubmit(const std::function<void(const Simulation &)> &on_submit);

    void render() override;

  private:
    std::function<void(const Simulation &)> on_submit;
    char date[20];
    Simulation simulation;

    double r0[3] = {1.0, 0.0, 0.0};
    double v0[3] = {0.0f, 1.0f, 0.0f};
    double r0_mag = 1.0f;
    double v0_mag = 1.0f;

    bool is_rel = false, is_quant = true, is_3d = true;
    int principal = 1, angular = 0, magnetic = 0;

    StateBuilder stateBuilder;

    std::vector<std::unique_ptr<Potential>> potentials;

    void resetSimulation();

    void updateSimulation();

    void formatName(char *name);
};

#endif