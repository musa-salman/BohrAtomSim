#pragma once

#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <memory>
#include <optional>

#include "simulation/model/Simulation.hpp"
#include "storage/dataset/Dataset.hpp"
#include "storage/persistence/SimulationResultMonitor.hpp"

namespace simulation::service {
using namespace simulation::model;
using namespace storage::dataset;
using namespace storage::persistence;

class SimulationService {
  public:
    virtual ~SimulationService() = default;

    [[nodiscard]] virtual size_t
    addSimulation(const Simulation &simulation) = 0;

    virtual void updateSimulation(const Simulation &simulation) = 0;

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

    [[nodiscard]] virtual const Dataset &
    getSimulationResult(size_t id) const = 0;

    [[nodiscard]] virtual std::optional<
        std::shared_ptr<SimulationResultMonitor>>
    getSimulationMonitor(size_t id) const = 0;
};
} // namespace simulation::service
