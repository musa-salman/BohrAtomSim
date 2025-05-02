#ifndef SIMULATION_REPOSITORY_HPP
#define SIMULATION_REPOSITORY_HPP

#include <memory>
#include <sqlite3.h>
#include <vector>

#include "SQLiteCpp/Database.h"
#include "simulation_repositories/ISimulationRepository.hpp"
#include "simulator_runner/Simulation.hpp"

class SimulationRepository : public ISimulationRepository {
  public:
    explicit SimulationRepository();

    void markSimulationComplete(size_t id) override;

    size_t add(const Simulation &simulation) override;

    void remove(size_t id) override;

    std::vector<std::shared_ptr<Simulation>> getAll() override;

  private:
    std::vector<std::shared_ptr<Simulation>> simulations;
    std::shared_ptr<SQLite::Database> db;
};

#endif // SIMULATION_REPOSITORY_HPP
