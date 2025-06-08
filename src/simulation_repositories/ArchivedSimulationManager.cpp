#include <format>
#include <string>

#include "dataset/DatasetFactory.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/SimulationResultLoader.hpp"

const dataset::Dataset &ArchivedSimulationManager::getSimulation(size_t id) {
    if (!simulationsResult.contains(id)) {
        auto archivedResults =
            std::unordered_map<std::string, std::vector<double>>();

        std::string filepath =
            std::format("{}/simulations/{}.bin", DB_PATH, id);

        SimulationResultLoader::loadSimulation(filepath, archivedResults);
        simulationsResult[id] =
            dataset::DatasetFactory::create(std::move(archivedResults));
    }

    return *simulationsResult[id];
}
