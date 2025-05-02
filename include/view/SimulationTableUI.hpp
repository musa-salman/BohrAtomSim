#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "simulation_repositories/ISimulationRepository.hpp"
#include "view/Component.hpp"

class SimulationTableUI : public Component {
  public:
    SimulationTableUI();

    void render() override;

  private:
    std::shared_ptr<ISimulationRepository> simulationRepository;

    std::vector<std::shared_ptr<Simulation>> simulations;
    std::string searchQuery;

    std::optional<size_t> selectedSimulationId;

    void renderTable();
    void renderSearchBar();
    void renderActionButtons();
};