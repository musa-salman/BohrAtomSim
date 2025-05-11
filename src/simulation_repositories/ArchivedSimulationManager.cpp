#include <algorithm>
#include <format>
#include <string>

#include "dataset/DatasetFactory.hpp"
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

const Dataset &ArchivedSimulationManager::getSimulation(size_t id) {
    if (!simulationsResult.contains(id)) {
        loadSimulations();
        auto archivedResults =
            std::unordered_map<std::string, std::vector<double>>();

        auto it = std::ranges::find_if(
            simulations, [id](const auto &s) { return s->getId() == id; });

        if (it != simulations.end()) {
            std::string filepath =
                std::format("{}/simulations/{}.bin", DB_PATH, id);

            SimulationResultLoader::loadSimulation(filepath, archivedResults);
        }

        simulationsResult[id] =
            DatasetFactory::create(std::move(archivedResults));
    }

    return *simulationsResult[id];
}
