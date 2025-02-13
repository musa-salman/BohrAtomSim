#ifndef SIMULATION_REPOSITORY_HPP
#define SIMULATION_REPOSITORY_HPP

#include <memory>
#include <sqlite3.h>
#include <vector>

#include "simulator_runner/Simulation.hpp"

class SimulationRepository {
  public:
    explicit SimulationRepository();

    ~SimulationRepository();

    size_t createSimulation(const Simulation &simulation);

    void markSimulationComplete(size_t id);

    void removeSimulation(size_t id);

    std::vector<std::shared_ptr<Simulation>> getSimulations(bool cached = true);

  private:
    std::vector<std::shared_ptr<Simulation>> simulations;
    sqlite3 *db;

    static int callback(void *data, int argc, char **argv, char **colNames);
};

#endif // SIMULATION_REPOSITORY_HPP
