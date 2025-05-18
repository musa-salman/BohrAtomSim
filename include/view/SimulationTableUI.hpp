#include <optional>
#include <string>

#include "view/Component.hpp"

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