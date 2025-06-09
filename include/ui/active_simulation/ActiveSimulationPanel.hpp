#pragma once

#include "components/AddSimulationDialog.hpp"
#include "ui/active_simulation/components/SimulationInspectorPanel.hpp"
#include "ui/components/SimulationListPanel.hpp"

namespace ui::active_simulation {

class ActiveSimulationPanel {
  public:
    ActiveSimulationPanel();

    void render();

  private:
    static bool _renderSimulationCard(const Simulation &simulation,
                                      bool isSelected) noexcept;
    size_t selectedSimulationId = 0;

    components::AddSimulationDialog addSimulationWindow;
    ui::components::SimulationListPanel listPanel;
    components::SimulationInspectorPanel inspectorPanel;
};

} // namespace ui::active_simulation
