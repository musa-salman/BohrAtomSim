#pragma once

#include <SQLiteCpp/Database.h>
#include <cstddef>
#include <memory>
#include <vector>

#include "SimulationRepository.hpp"
#include "simulation/model/Simulation.hpp"

namespace storage::persistence {

class SimulationRepositoryImpl : public SimulationRepository {
  public:
    explicit SimulationRepositoryImpl();

    void completeSimulation(size_t id) const override;

    size_t add(const simulation::model::Simulation &simulation) const override;

    void update(const simulation::model::Simulation &simulation) override;

    void remove(size_t id) const override;

    std::vector<std::unique_ptr<simulation::model::Simulation>>
    getAll() const override;

  private:
    std::shared_ptr<SQLite::Database> db;
};
} // namespace storage::persistence
