#include <algorithm>
#include <format>
#include <string>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/SimulationResultLoader.hpp"

ArchivedSimulationManager::ArchivedSimulationManager()
    : simulationRepository(
          ServiceLocator::getInstance().get<ISimulationRepository>()) {
    loadSimulations();
}

void ArchivedSimulationManager::loadSimulations() {
    simulations = simulationRepository->getAll();
}

std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
ArchivedSimulationManager::getSimulation(size_t id) {
    if (!simulations_result.contains(id)) {
        loadSimulations();
        auto dataset = std::make_shared<
            std::unordered_map<std::string, std::vector<double>>>();
        simulations_result[id] = dataset;

        auto it = std::ranges::find_if(
            simulations, [id](const auto &s) { return s->getId() == id; });

        if (it != simulations.end()) {
            std::string filepath =
                std::format("{}/simulations/{}.bin", DB_PATH, id);

            SimulationResultLoader::loadSimulation(filepath, *dataset);
        }
    }

    return simulations_result[id];
}
