#ifndef SIMULATION_REPOSITORY_HPP
#define SIMULATION_REPOSITORY_HPP

#include "simulation_repositories/Repository.hpp"
#include "simulator_runner/Simulation.hpp"

class SimulationRepository : public Repository<Simulation> {
  public:
    virtual void markSimulationComplete(size_t id) const = 0;
};

#endif // SIMULATION_REPOSITORY_HPP