#include <boost/asio.hpp>
#include <thread>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulator_runner/Simulator.hpp"

#include "simulator_runner/Simulation.hpp"

#include "utils/utils.h"

Simulator::Simulator()
    : ioContext(1), workGuard(boost::asio::make_work_guard(ioContext)) {
    workers.emplace_back([this]() { ioContext.run(); });
}

Simulator::Simulator(int numThreads)
    : ioContext(numThreads),
      workGuard(boost::asio::make_work_guard(ioContext)) {
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this]() { ioContext.run(); });
    }
}

void Simulator::pauseSimulation(size_t id) {
    auto it = steppers.find(id);
    if (it != steppers.end())
        it->second->pause();
}

void Simulator::resumeSimulation(size_t id) {
    auto it = steppers.find(id);
    if (it != steppers.end())
        boost::asio::post(ioContext, [it]() { it->second->resume(); });
}

void Simulator::stopSimulation(size_t id) {
    auto it = steppers.find(id);
    if (it != steppers.end()) {
        it->second->stop();
        steppers.erase(it);
    }
}

Simulator::~Simulator() {
    ioContext.stop();
    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void Simulator::simulateOrbit(Simulation &simulation,
                              std::function<void()> onCompletion) {

    char output_filename[FILE_PATH_SIZE]; // NOSONAR
    format_output_filename(simulation.id, output_filename);
    FILE *file_bin = fopen(output_filename, "a+b");

    if (file_bin == nullptr)
        throw std::runtime_error("Failed to open file for writing");

    auto ss2d = std::make_shared<SimulationStepper2D>(simulation, onCompletion,
                                                      file_bin);
    steppers[simulation.id] = ss2d;

    boost::asio::post(ioContext, [ss2d]() { ss2d->run(); });
}
