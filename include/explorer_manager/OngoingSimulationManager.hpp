#ifndef ONGOING_SIMULATION_MANAGER_HPP
#define ONGOING_SIMULATION_MANAGER_HPP

#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>

#include "simulation_repositories/SimulationResultMonitor.hpp"

class OngoingSimulationManager {
  private:
    std::unordered_map<size_t, std::shared_ptr<SimulationResultMonitor>>
        monitors;

  public:
    void startMonitoring(size_t id);

    void pauseMonitoring(size_t id);

    void stopMonitoring(size_t id);

    std::optional<std::shared_ptr<SimulationResultMonitor>>
    getMonitor(size_t id);
};

#endif // ONGOING_SIMULATION_MANAGER_HPP
