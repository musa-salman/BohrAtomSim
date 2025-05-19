
#include "steppers/SimulationStepper.hpp"

#include <cstdio>
#include <thread>

#include "steppers/SimulationStepper.hpp"

SimulationStepper::~SimulationStepper() {
    if (file_bin) {
        fclose(file_bin);
        file_bin = nullptr;
    }
}

void SimulationStepper::pause() {
    if (isRunning)
        isRunning = false;
}

void SimulationStepper::resume() {
    if (!isRunning) {
        isRunning = true;
        run();
    }
}

void SimulationStepper::stop() {
    if (isRunning) {
        isRunning = false;
        isStopped = true;

        while (!isFinished)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
