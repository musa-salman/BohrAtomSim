#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "simulator_runner/Simulation.hpp"
#include "simulator_runner/Simulator.hpp"
#include "steppers/SimulationStepper.hpp"
#include "utils/utils.h"

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
                              std::function<void()> &&onCompletion) {
    const size_t id = simulation.getId();
    char output_filename[FILE_PATH_SIZE]; // NOSONAR
    format_output_filename(id, output_filename);
    FILE *file_bin = fopen(output_filename, "wb");

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

    boost::asio::post(ioContext, [this, id]() { this->steppers[id]->run(); });
}
