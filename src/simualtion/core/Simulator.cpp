#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "physics/stepper/SimulationStepper.hpp"
#include "simulation/core/Simulator.hpp"
#include "simulation/model/Simulation.hpp"
#include "utils/utils.hpp"

namespace simulation::core {

using namespace simulation::model;
using namespace physics::stepper;
using namespace utils;
using namespace boost::asio;

Simulator::Simulator(int numThreads)
    : ioContext(numThreads), workGuard(make_work_guard(ioContext)) {
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this]() { ioContext.run(); });
    }
}

void Simulator::pauseSimulation(size_t id) {
    auto it = steppers.find(id);
    if (it != steppers.end())
        it->second->pause();
}

void Simulator::resumeSimulation(Simulation &simulation) {
    const size_t id = simulation.getId();
    auto it = steppers.find(id);
    if (it != steppers.end()) {
        simulation.setStatus(Simulation::SimulationStatus::QUEUED);

        post(ioContext, [it, &simulation]() {
            simulation.setStatus(Simulation::SimulationStatus::RUNNING);
            it->second->resume();
        });
    }
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
                              std::function<void()> &&onCompletion) {
    const size_t id = simulation.getId();
    const std::string outputFilename = formatOutputFilename(id);
    FILE *file_bin = fopen(outputFilename.c_str(), "wb");

    if (file_bin == nullptr)
        throw std::runtime_error("Failed to open file for writing");

    StepperCommonConfig commonConfig{
        .delta_time = simulation.getDeltaTime(),
        .total_duration = simulation.getTotalDuration(),
        .onCompletion = std::move(onCompletion),
        .record_interval = simulation.getRecordInterval(),
        .rLocalMaxLimit = simulation.getRLocalMaxCountLimit(),
        .file_bin = file_bin,
    };

    const bool is3D = simulation.getIs3D();
    const bool isRel = simulation.getIsRelativistic();
    const bool isQuant = simulation.getIsQuantized();

    if (is3D) {
        if (isRel && isQuant) {
            make_state<true, true, true>(simulation, commonConfig);
        } else if (isRel && !isQuant) {
            make_state<true, true, false>(simulation, commonConfig);
        } else if (!isRel && isQuant) {
            make_state<true, false, true>(simulation, commonConfig);
        } else {
            make_state<true, false, false>(simulation, commonConfig);
        }
    } else {
        if (isRel && isQuant) {
            make_state<false, true, true>(simulation, commonConfig);
        } else if (isRel && !isQuant) {
            make_state<false, true, false>(simulation, commonConfig);
        } else if (!isRel && isQuant) {
            make_state<false, false, true>(simulation, commonConfig);
        } else {
            make_state<false, false, false>(simulation, commonConfig);
        }
    }

    simulation.setStatus(Simulation::SimulationStatus::QUEUED);
    post(ioContext, [this, &simulation]() {
        simulation.status = Simulation::SimulationStatus::RUNNING;
        this->steppers[simulation.getId()]->run();
    });
}
} // namespace simulation::core
