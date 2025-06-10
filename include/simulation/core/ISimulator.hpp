#pragma once

#include <cstddef>
#include <functional>

#include "simulation/model/Simulation.hpp"

namespace simulation::core {
class ISimulator {
  public:
    virtual ~ISimulator() = default;

    virtual void simulateOrbit(model::Simulation &simulation,
                               std::function<void()> &&onCompletion) = 0;

    virtual void pauseSimulation(size_t id) = 0;
    virtual void resumeSimulation(model::Simulation &simulation) = 0;
    virtual void stopSimulation(size_t id) = 0;
};
} // namespace simulation::core
