#ifndef SIMULATION_SERVICE_HPP
#define SIMULATION_SERVICE_HPP

#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>

#include "dataset/Dataset.hpp"
#include "simulation_repositories/SimulationResultMonitor.hpp"
#include "simulator_runner/Simulation.hpp"

class SimulationService {
  public:
    virtual ~SimulationService() = default;

    [[nodiscard]] virtual size_t
    addSimulation(const Simulation &simulation) = 0;

    virtual void removeSimulation(size_t id) = 0;

    virtual void startSimulation(size_t id) = 0;
    virtual void pauseSimulation(size_t id) = 0;
    virtual void stopSimulation(size_t id) = 0;
    virtual void resumeSimulation(size_t id) = 0;

    [[nodiscard]] virtual const std::unordered_map<
        size_t, std::unique_ptr<Simulation>> &
    getOngoingSimulations() const = 0;

    [[nodiscard]] virtual const std::unordered_map<
        size_t, std::unique_ptr<Simulation>> &
    getCompletedSimulations() const = 0;

    [[nodiscard]] virtual const Dataset &
    getSimulationResult(size_t id) const = 0;

    [[nodiscard]] virtual std::optional<
        std::shared_ptr<SimulationResultMonitor>>
    getSimulationMonitor(size_t id) const = 0;
};

#endif // SIMULATION_SERVICE_HPP