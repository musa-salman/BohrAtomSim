#ifndef ARCHIVED_SIMULATION_MANAGER_HPP
#define ARCHIVED_SIMULATION_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "dataset/Dataset.hpp"
#include "simulation_repositories/ISimulationRepository.hpp"
#include "simulator_runner/Simulation.hpp"

class ArchivedSimulationManager {
  public:
    explicit ArchivedSimulationManager();

    std::vector<std::shared_ptr<Simulation>> getSimulations() const {
        return simulations;
    }

    [[nodiscard]] const Dataset &getSimulation(size_t id);

  private:
    void loadSimulations();

    std::unordered_map<size_t, std::unique_ptr<Dataset>> simulationsResult;

    std::vector<std::shared_ptr<Simulation>> simulations;
    std::shared_ptr<ISimulationRepository> simulationRepository;
};

#endif // ARCHIVED_SIMULATION_MANAGER_HPP