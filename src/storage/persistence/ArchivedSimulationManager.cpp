#include <format>
#include <string>

#include "storage/dataset/Dataset.hpp"
#include "storage/dataset/DatasetFactory.hpp"
#include "storage/persistence/ArchivedSimulationManager.hpp"
#include "storage/persistence/SimulationResultLoader.hpp"

namespace storage::persistence {
using namespace storage::dataset;

const Dataset &ArchivedSimulationManager::getSimulation(size_t id) {
    if (!simulationsResult.contains(id)) {
        auto archivedResults =
            std::unordered_map<std::string, std::vector<double>>();

        std::string filepath =
            std::format("{}/simulations/{}.bin", DB_PATH, id);

        SimulationResultLoader::loadSimulation(filepath, archivedResults);
        simulationsResult[id] =
            DatasetFactory::create(std::move(archivedResults));
    }

    return *simulationsResult[id];
}
} // namespace storage::persistence
