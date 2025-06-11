#pragma once

#include <functional>

#include <vector>

#include "simulation/factories/SimulationBuilder.hpp"
#include "simulation/model/Potential.hpp"
#include "simulation/model/Simulation.hpp"
#include "ui/components/Component.hpp"

namespace ui::components {

class SimulationEditorDialog : public ui::components::Component {
  public:
    explicit SimulationEditorDialog();

    void
    setOnSubmit(std::function<void(simulation::model::Simulation)> &&onSubmit);
    void setSimulation(const simulation::model::Simulation &simulation);
    void render() override;

  private:
    bool m_isDialogOpen;
    bool m_isAutoName;
    bool m_isRLocalMaxLimited;

    std::function<void(simulation::model::Simulation)> m_onSubmit;

    simulation::factories::SimulationBuilder m_simulationBuilder;

    std::vector<std::unique_ptr<simulation::model::Potential>> m_potentials;

    void _loadPotentials();
    static inline bool
    _isLocked(const simulation::model::Simulation::SimulationStatus
                  &simulationStatus) noexcept {
        return simulationStatus ==
                   simulation::model::Simulation::SimulationStatus::RUNNING ||
               simulationStatus ==
                   simulation::model::Simulation::SimulationStatus::PAUSED ||
               simulationStatus ==
                   simulation::model::Simulation::SimulationStatus::QUEUED ||
               simulationStatus ==
                   simulation::model::Simulation::SimulationStatus::COMPLETED;
    }

    void _renderStateConfiguration(bool isLocked);
    void _renderPotentialConfiguration(bool isLocked);
    void _renderSimulationConfiguration(bool isLocked);
};

} // namespace ui::components
