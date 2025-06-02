
#include "steppers/SimulationStepper.hpp"

#include <cstdio>
#include <thread>

#include "steppers/SimulationStepper.hpp"

SimulationStepper::~SimulationStepper() {
    if (file_bin) {
        fflush(file_bin);
        fsync(fileno(file_bin));
        fclose(file_bin);
        file_bin = nullptr;
    }
}

void SimulationStepper::run() {
    if (isFinished || isStopped)
        return;

    isRunning = true;

    executeSimulationLoop();

    if (it * delta_time >= total_duration || ((rLocalMaxCount >= rLocalMaxLimit) && (rLocalMaxCount > 0)) || isStopped) {
        if (file_bin) {
            fflush(file_bin);
            fsync(fileno(file_bin));
            fclose(file_bin);
            file_bin = nullptr;
        }
        onCompletion();
        isFinished = true;
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
    } else if (!isFinished) {
        isStopped = true;
        isFinished = true;
        if (file_bin) {
            fflush(file_bin);
            fsync(fileno(file_bin));
            fclose(file_bin);
            file_bin = nullptr;
        }
        onCompletion();
    }
}
