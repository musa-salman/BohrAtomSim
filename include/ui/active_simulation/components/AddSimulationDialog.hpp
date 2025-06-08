#ifndef UI_ACTIVE_SIMULATION_ADD_SIMULATION_DIALOG_HPP
#define UI_ACTIVE_SIMULATION_ADD_SIMULATION_DIALOG_HPP

#include <functional>

#include <vector>

#include "simulator_runner/Simulation.hpp"
#include "steppers/StateBuilder.hpp"
#include "ui/components/Component.hpp"

namespace ui::active_simulation::components {

class AddSimulationDialog : public ui::components::Component {
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
};

} // namespace ui::active_simulation::components

#endif // UI_ACTIVE_SIMULATION_ADD_SIMULATION_DIALOG_HPP