#ifndef ARCHIVED_SIMULATION_MANAGER_HPP
#define ARCHIVED_SIMULATION_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "dataset/Dataset.hpp"

class ArchivedSimulationManager {
  public:
    [[nodiscard]] const dataset::Dataset &getSimulation(size_t id);

  private:
    std::unordered_map<size_t, std::unique_ptr<dataset::Dataset>>
        simulationsResult;
};

#endif // ARCHIVED_SIMULATION_MANAGER_HPP