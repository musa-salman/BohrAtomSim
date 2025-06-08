#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>

#include "explorer_manager/OngoingSimulationManager.hpp"
#include "utils/utils.hpp"

void OngoingSimulationManager::startMonitoring(size_t id) {
    if (!monitors.contains(id)) {

        auto monitor = std::make_shared<SimulationResultMonitor>(
            utils::formatOutputFilename(id, DB_PATH));
        monitors[id] = monitor;
    }

    monitors[id]->startMonitoring();
}

void OngoingSimulationManager::pauseMonitoring(size_t id) {
    if (!monitors.contains(id)) {
        throw std::runtime_error("Monitor not found");
    }

    monitors[id]->pauseMonitoring();
}

void OngoingSimulationManager::stopMonitoring(size_t id) {
    if (!monitors.contains(id)) {
        throw std::runtime_error("Monitor not found");
    }

    monitors[id]->pauseMonitoring();
    monitors.erase(id);
}

std::optional<std::shared_ptr<SimulationResultMonitor>>
OngoingSimulationManager::getMonitor(size_t id) {
    if (!monitors.contains(id))
        return std::nullopt;

    return monitors[id];
}
