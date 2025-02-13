#ifndef ARCHIVED_SIMULATION_MANAGER_HPP
#define ARCHIVED_SIMULATION_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "simulation_repositories/SimulationRepository.hpp"
#include "simulator_runner/Simulation.hpp"

class ArchivedSimulationManager {
  public:
    explicit ArchivedSimulationManager(SimulationRepository &db);

    std::vector<std::shared_ptr<Simulation>> getSimulations() const {
        return simulations;
    }

    std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
    getSimulation(size_t id);

  private:
    void loadSimulations();

    std::unordered_map<
        size_t,
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>
        simulations_result;

    std::vector<std::shared_ptr<Simulation>> simulations;
    SimulationRepository &db;
};

#endif // ARCHIVED_SIMULATION_MANAGER_HPP