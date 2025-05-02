#ifndef ONGOING_SIMULATION_MANAGER_HPP
#define ONGOING_SIMULATION_MANAGER_HPP

#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ISimulationRepository.hpp"
#include "simulation_repositories/SimulationResultMonitor.hpp"
#include "simulator_runner/Simulation.hpp"

class OngoingSimulationManager {
  private:
    std::shared_ptr<ISimulationRepository> simulationRepository;
    std::unordered_map<size_t, std::shared_ptr<Simulation>> simulations;
    std::unordered_map<size_t, std::shared_ptr<SimulationResultMonitor>>
        monitors;

  public:
    explicit OngoingSimulationManager()
        : simulationRepository(
              ServiceLocator::getInstance().get<ISimulationRepository>()) {}

    void addSimulation(const Simulation &simulation);

    void markSimulationAsComplete(size_t id);

    std::optional<std::shared_ptr<SimulationResultMonitor>>
    getMonitor(size_t id);

    const std::unordered_map<size_t, std::shared_ptr<Simulation>> &
    getSimulations() const;
};

#endif // ONGOING_SIMULATION_MANAGER_HPP
