#pragma once

#include "Repository.hpp"
#include "simulation/model/Simulation.hpp"

namespace storage::persistence {

class SimulationRepository : public Repository<simulation::model::Simulation> {
  public:
    virtual void completeSimulation(size_t id) const = 0;
    virtual void update(const simulation::model::Simulation &simulation) = 0;
};
} // namespace storage::persistence
