#ifndef ISIMULATOR_HPP
#define ISIMULATOR_HPP

#include <cstddef>
#include <functional>

#include "simulator_runner/Simulation.hpp"

class ISimulator {
  public:
    virtual ~ISimulator() = default;

    virtual void simulateOrbit(Simulation &simulation,
                               std::function<void()> onCompletion) = 0;

    virtual void pauseSimulation(size_t id) = 0;
    virtual void resumeSimulation(size_t id) = 0;
    virtual void stopSimulation(size_t id) = 0;
};

#endif // ISIMULATOR_HPP