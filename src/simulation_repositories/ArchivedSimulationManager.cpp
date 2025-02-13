#include <algorithm>
#include <format>
#include <string>

#include "explorer_manager/SimulationFields.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/SimulationResultLoader.hpp"

ArchivedSimulationManager::ArchivedSimulationManager(SimulationRepository &db)
    : db(db) {
    loadSimulations();
}

void ArchivedSimulationManager::loadSimulations() {
    simulations = db.getSimulations(false);
}

std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
ArchivedSimulationManager::getSimulation(size_t id) {
    if (!simulations_result.contains(id)) {
        loadSimulations();
        auto dataset = std::make_shared<
            std::unordered_map<std::string, std::vector<double>>>();
        simulations_result[id] = dataset;

        auto it = std::ranges::find_if(
            simulations, [id](const auto &s) { return s->id == id; });

        if (it != simulations.end()) {
            int type = it->get()->type;

            std::string filepath =
                std::format("{}/simulations/{}.bin", DB_PATH, id);

            SimulationResultLoader::loadSimulation(
                filepath, getColumnNames(type), *dataset);
        }
    }

    return simulations_result[id];
}
