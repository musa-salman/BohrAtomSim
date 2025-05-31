#ifndef SIMULATION_STEPPER_HPP
#define SIMULATION_STEPPER_HPP

#include <cmath>
#include <cstdio>
#include <functional>

#include "eom/utils.hpp"
#include "utils/types.h"

struct StepperCommonConfig {
    scalar delta_time;
    scalar total_duration;
    std::function<void()> onCompletion;
    uint record_interval;

    FILE *file_bin;
};

class SimulationStepper {
  protected:
    const scalar delta_time;
    const scalar total_duration;
    const uint record_interval;

    FILE *file_bin;
    bool isRunning = false;
    bool isFinished = false;
    bool isStopped = false;

    std::function<void()> onCompletion;

    size_t it = 0;

    virtual void executeSimulationLoop() = 0;

  public:
    SimulationStepper(const StepperCommonConfig &config)
        : delta_time(config.delta_time), total_duration(config.total_duration),
          record_interval(config.record_interval), file_bin(config.file_bin),
          onCompletion(config.onCompletion){};

    virtual ~SimulationStepper();

    void run();

    void pause();

    void resume();

    void stop();
};

#endif // SIMULATION_2D_STEPPER_HPP