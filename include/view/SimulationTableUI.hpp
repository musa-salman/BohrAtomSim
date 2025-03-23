#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "simulation_repositories/SimulationRepository.hpp"
#include "view/Component.hpp"

class SimulationTableUI : public Component {
  public:
    SimulationTableUI(SimulationRepository &repo);

    void render() override;

  private:
    SimulationRepository &repository;

    std::vector<std::shared_ptr<Simulation>> simulations;
    std::string searchQuery;

    std::optional<size_t> selectedSimulationId;

    void renderTable();
    void renderSearchBar();
    void renderActionButtons();
};