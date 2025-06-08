#pragma once

#include <cstddef>
#include <functional>

#include "AddSimulationDialog.hpp"

namespace ui::active_simulation::components {

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

} // namespace ui::active_simulation::components
