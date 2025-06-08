#ifndef SIMULATION_REPOSITORY_IMPL_HPP
#define SIMULATION_REPOSITORY_IMPL_HPP

#include <cstddef>
#include <memory>
#include <sqlite3.h>
#include <vector>

#include "SQLiteCpp/Database.h"
#include "simulation_repositories/SimulationRepository.hpp"
#include "simulator_runner/Simulation.hpp"

class SimulationRepositoryImpl : public SimulationRepository {
  public:
    explicit SimulationRepositoryImpl();

    void completeSimulation(size_t id) const override;

    size_t add(const Simulation &simulation) const override;

    void remove(size_t id) const override;

    std::vector<std::unique_ptr<Simulation>> getAll() const override;

  private:
    std::shared_ptr<SQLite::Database> db;
};

#endif // SIMULATION_REPOSITORY_IMPL_HPP
