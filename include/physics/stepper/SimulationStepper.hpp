#pragma once

#include <cmath>
#include <cstdio>
#include <functional>

#include "physics/math/types.hpp"

namespace physics::stepper {

struct StepperCommonConfig {
    math::scalar delta_time;
    math::scalar total_duration;
    std::function<void()> onCompletion;
    uint record_interval;
    int rLocalMaxLimit;

    FILE *file_bin;
};

class SimulationStepper {
  protected:
    const math::scalar delta_time;
    const math::scalar total_duration;
    const uint record_interval;
    const int rLocalMaxLimit;

    FILE *file_bin;
    bool isRunning = false;
    bool isFinished = false;
    bool isStopped = false;

    std::function<void()> onCompletion;

    size_t it = 0;
    int rLocalMaxCount = 0;

    virtual void executeSimulationLoop() = 0;

  public:
    SimulationStepper(const StepperCommonConfig &config)
        : delta_time(config.delta_time), total_duration(config.total_duration),
          record_interval(config.record_interval),
          rLocalMaxLimit(config.rLocalMaxLimit), file_bin(config.file_bin),
          onCompletion(config.onCompletion){};

    virtual ~SimulationStepper();

    void run();

    void pause();

    void resume();

    void stop();
};

} // namespace physics::stepper