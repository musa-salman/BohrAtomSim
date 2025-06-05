#ifndef UI_ACTIVE_SIMULATION_SIMULATION_LIST_PANEL_HPP
#define UI_ACTIVE_SIMULATION_SIMULATION_LIST_PANEL_HPP

#include <cstddef>
#include <functional>

#include "AddSimulationDialog.hpp"

namespace ui::active_simulation {

class SimulationListPanel {
  public:
    using OnSelectCallback = std::function<void(size_t)>;

    SimulationListPanel(AddSimulationDialog &dialog);

    void setOnSelect(OnSelectCallback cb);
    void render();

  private:
    AddSimulationDialog &dialog;
    OnSelectCallback onSelect;
    size_t selectedSimulationId = 0;
};

} // namespace ui::active_simulation

#endif // UI_ACTIVE_SIMULATION_SIMULATION_LIST_PANEL_HPP