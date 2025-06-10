#pragma once

#include <functional>

#include <vector>

#include "simulation/factories/SimulationBuilder.hpp"
#include "simulation/model/Potential.hpp"
#include "simulation/model/Simulation.hpp"
#include "ui/components/Component.hpp"

namespace ui::active_simulation::components {

class AddSimulationDialog : public ui::components::Component {
  public:
    explicit AddSimulationDialog();

    void setOnSubmit(
        std::function<void(const simulation::model::Simulation &)> &&onSubmit);

    void render() override;

  private:
    bool m_isDialogOpen;
    std::function<void(const simulation::model::Simulation &)> m_onSubmit;

    simulation::factories::SimulationBuilder m_simulationBuilder;

    std::vector<std::unique_ptr<simulation::model::Potential>> m_potentials;

    void _loadPotentials();

    void _renderStateConfiguration();
    void _renderPotentialConfiguration();
    void _renderSimulationConfiguration();
};

} // namespace ui::active_simulation::components
