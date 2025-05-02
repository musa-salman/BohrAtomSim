#ifndef ISIMULATION_REPOSITORY_HPP
#define ISIMULATION_REPOSITORY_HPP

#include "simulation_repositories/Repository.hpp"
#include "simulator_runner/Simulation.hpp"

class ISimulationRepository : public Repository<Simulation> {
  public:
    virtual void markSimulationComplete(size_t id) = 0;
};

#endif // ISIMULATION_REPOSITORY_HPP