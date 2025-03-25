#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "explorer_manager/OngoingSimulationManager.hpp"
#include "explorer_manager/SimulationFields.hpp"
#include "utils/utils.h"

void OngoingSimulationManager::addSimulation(const Simulation &simulation) {
    const std::vector<std::string> fields = getColumnNames(simulation.type);
    size_t id = this->simulation_repository.createSimulation(simulation);
    if (id == 0) {
        throw std::runtime_error("Failed to create simulation");
    }

    auto _simulation = std::make_shared<Simulation>(
        simulation.name, id, simulation.orbit, simulation.type,
        simulation.record_interval, simulation.revolutions,
        simulation.time_interval);

    char output_filename[FILE_PATH_SIZE]; // NOSONAR
    format_output_filename(id, output_filename);

    auto _monitor =
        std::make_shared<SimulationResultMonitor>(std::string(output_filename));

    simulations.try_emplace(_simulation->id, _simulation);
    monitors.try_emplace(_simulation->id, _monitor);
}

std::optional<std::shared_ptr<SimulationResultMonitor>>
OngoingSimulationManager::getMonitor(size_t id) {
    if (!monitors.contains(id)) {
        throw std::invalid_argument("Simulation not found");
    }

    return monitors[id];
}

void OngoingSimulationManager::markSimulationAsComplete(size_t id) {
    simulation_repository.markSimulationComplete(id);
}

const std::unordered_map<size_t, std::shared_ptr<Simulation>> &
OngoingSimulationManager::getSimulations() const {
    return simulations;
}