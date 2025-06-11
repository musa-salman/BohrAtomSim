#pragma once

#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <functional>
#include <memory>

#include "simulation/core/ISimulator.hpp"
#include "simulation/model/Simulation.hpp"
#include "simulation/service/SimulationService.hpp"
#include "storage/dataset/Dataset.hpp"
#include "storage/persistence/ArchivedSimulationManager.hpp"
#include "storage/persistence/SimulationMonitorManager.hpp"
#include "storage/persistence/SimulationRepository.hpp"

namespace simulation::service {
using namespace simulation::model;
using namespace simulation::core;
using namespace storage::dataset;
using namespace storage::persistence;

class SimulationServiceImpl : public SimulationService {
  public:
    explicit SimulationServiceImpl();

    [[nodiscard]] size_t addSimulation(const Simulation &simulation) override;

    void updateSimulation(const Simulation &simulation) override;

    void removeSimulation(size_t id) override;

    void startSimulation(size_t id) override;
    void pauseSimulation(size_t id) override;
    void stopSimulation(size_t id) override;
    void resumeSimulation(size_t id) override;

    [[nodiscard]] const boost::container::flat_map<
        size_t, std::shared_ptr<Simulation>, std::greater<size_t>> &
    getOngoingSimulations() const override;

    [[nodiscard]]
    const boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                                     std::greater<size_t>> &
    getCompletedSimulations() const override;

    [[nodiscard]] const Dataset &getSimulationResult(size_t id) const override;

    [[nodiscard]] std::optional<std::shared_ptr<SimulationResultMonitor>>
    getSimulationMonitor(size_t id) const override;

  private:
    SimulationRepository &simulationRepository;
    ISimulator &simulator;
    SimulationMonitorManager &simulationMonitorManager;
    ArchivedSimulationManager &archivedSimulationManager;

    boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                               std::greater<size_t>>
        ongoingSimulations;

    boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                               std::greater<size_t>>
        completedSimulations;
};
} // namespace simulation::service
