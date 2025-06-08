#ifndef SIMULATION_SERVICE_IMPL_HPP
#define SIMULATION_SERVICE_IMPL_HPP

#include <cstddef>
#include <memory>
#include <unordered_map>

#include "dataset/Dataset.hpp"
#include "explorer_manager/OngoingSimulationManager.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/SimulationRepository.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "simulator_runner/ISimulator.hpp"
#include "simulator_runner/Simulation.hpp"

class SimulationServiceImpl : public SimulationService {
  public:
    explicit SimulationServiceImpl();

    [[nodiscard]] size_t addSimulation(const Simulation &simulation) override;

    void removeSimulation(size_t id) override;

    void startSimulation(size_t id) override;
    void pauseSimulation(size_t id) override;
    void stopSimulation(size_t id) override;
    void resumeSimulation(size_t id) override;

    [[nodiscard]] const std::unordered_map<size_t,
                                           std::shared_ptr<Simulation>> &
    getOngoingSimulations() const override;

    [[nodiscard]] const std::unordered_map<size_t,
                                           std::shared_ptr<Simulation>> &
    getCompletedSimulations() const override;

    [[nodiscard]] const dataset::Dataset &
    getSimulationResult(size_t id) const override;

    [[nodiscard]] std::optional<std::shared_ptr<SimulationResultMonitor>>
    getSimulationMonitor(size_t id) const override;

  private:
    const SimulationRepository &simulationRepository;
    ISimulator &simulator;
    OngoingSimulationManager &ongoingSimulationManager;
    ArchivedSimulationManager &archivedSimulationManager;

    std::unordered_map<size_t, std::shared_ptr<Simulation>> ongoingSimulations;

    std::unordered_map<size_t, std::shared_ptr<Simulation>>
        completedSimulations;
};

#endif // SIMULATION_SERVICE_IMPL_HPP