#pragma once

#include <memory>
#include <unordered_map>

#include "storage/dataset/Dataset.hpp"

namespace storage::persistence {

class ArchivedSimulationManager {
  public:
    [[nodiscard]] const dataset::Dataset &getSimulation(size_t id);

  private:
    std::unordered_map<size_t, std::unique_ptr<dataset::Dataset>>
        simulationsResult;
};
} // namespace storage::persistence
