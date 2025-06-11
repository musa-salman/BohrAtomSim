#pragma once

#include "simulation/model/Simulation.hpp"
#include "ui/active_simulation/components/SimulationInspectorPanel.hpp"
#include "ui/components/SimulationEditorDialog.hpp"
#include "ui/components/SimulationListPanel.hpp"

namespace ui::active_simulation {

class ActiveSimulationPanel {
  public:
    ActiveSimulationPanel();

    void render();

  private:
    static bool
    _renderSimulationCard(const simulation::model::Simulation &simulation,
                          bool isSelected) noexcept;
    size_t selectedSimulationId = 0;

    ui::components::SimulationEditorDialog addSimulationWindow;
    ui::components::SimulationListPanel listPanel;
    components::SimulationInspectorPanel inspectorPanel;
};

} // namespace ui::active_simulation
