#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>

#include "storage/persistence/SimulationResultMonitor.hpp"

namespace storage::persistence {

class SimulationMonitorManager {
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
} // namespace storage::persistence
