#ifndef SIMULATION_SERVICE_HPP
#define SIMULATION_SERVICE_HPP

#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <memory>
#include <optional>

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

    [[nodiscard]] virtual const boost::container::flat_map<
        size_t, std::shared_ptr<Simulation>, std::greater<size_t>> &
    getOngoingSimulations() const = 0;

    [[nodiscard]] virtual const boost::container::flat_map<
        size_t, std::shared_ptr<Simulation>, std::greater<size_t>> &
    getCompletedSimulations() const = 0;

    [[nodiscard]] virtual const dataset::Dataset &
    getSimulationResult(size_t id) const = 0;

    [[nodiscard]] virtual std::optional<
        std::shared_ptr<SimulationResultMonitor>>
    getSimulationMonitor(size_t id) const = 0;
};

#endif // SIMULATION_SERVICE_HPP