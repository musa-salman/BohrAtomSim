#pragma once

#include "components/AddSimulationDialog.hpp"
#include "ui/active_simulation/components/SimulationInspectorPanel.hpp"
#include "ui/active_simulation/components/SimulationListPanel.hpp"

namespace ui::active_simulation {

class ActiveSimulationPanel {
  public:
    ActiveSimulationPanel();

    void render();

  private:
    size_t selectedSimulationId = 0;

    components::AddSimulationDialog addSimulationWindow;
    components::SimulationListPanel listPanel;
    components::SimulationInspectorPanel inspectorPanel;
};

} // namespace ui::active_simulation
