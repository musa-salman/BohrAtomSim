#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>

#include "storage/persistence/SimulationMonitorManager.hpp"
#include "storage/persistence/SimulationResultMonitor.hpp"
#include "utils/utils.hpp"

namespace storage::persistence {

void SimulationMonitorManager::startMonitoring(size_t id) {
    if (!monitors.contains(id)) {

        auto monitor = std::make_shared<SimulationResultMonitor>(
            utils::formatOutputFilename(id));
        monitors[id] = monitor;
    }

    monitors[id]->startMonitoring();
}

void SimulationMonitorManager::pauseMonitoring(size_t id) {
    if (!monitors.contains(id)) {
        throw std::runtime_error("Monitor not found");
    }

    monitors[id]->pauseMonitoring();
}

void SimulationMonitorManager::stopMonitoring(size_t id) {
    if (!monitors.contains(id)) {
        throw std::runtime_error("Monitor not found");
    }

    monitors[id]->pauseMonitoring();
    monitors.erase(id);
}

std::optional<std::shared_ptr<SimulationResultMonitor>>
SimulationMonitorManager::getMonitor(size_t id) {
    if (!monitors.contains(id))
        return std::nullopt;

    return monitors[id];
}
} // namespace storage::persistence
