#ifndef UI_SIMULATION_TABLE_UI
#define UI_SIMULATION_TABLE_UI

#include <optional>
#include <string>

#include "ui/Component.hpp"

namespace ui {

class SimulationTableUI : public Component {
  public:
    SimulationTableUI();

    void render() override;

  private:
    std::string searchQuery;

    std::optional<size_t> selectedSimulationId;

    void renderTable();
    void renderSearchBar();
    void renderActionButtons();
};
} // namespace ui

#endif