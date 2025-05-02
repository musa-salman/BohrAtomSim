#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "atom/atom_bohr_sim.h"
#include "simulation_2d/SimulationStepper2D.hpp"
#include "simulator_runner/ISimulator.hpp"
#include "simulator_runner/Simulation.hpp"

class Simulator : public ISimulator {
  public:
    explicit Simulator(int numThreads);

    ~Simulator();

    void simulateOrbit(Simulation &simulation,
                       std::function<void()> onCompletion);

    void pauseSimulation(size_t id);

    void resumeSimulation(size_t id);

    void stopSimulation(size_t id);

  private:
    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        workGuard;
    std::vector<std::jthread> workers;

    std::unordered_map<size_t, std::shared_ptr<SimulationStepper2D>> steppers;
};

#endif // SIMULATOR_HPP