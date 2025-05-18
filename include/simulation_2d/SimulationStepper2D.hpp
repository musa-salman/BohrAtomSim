#ifndef SIMULATION_STEPPER_2D_HPP
#define SIMULATION_STEPPER_2D_HPP

#include <cstdio>
#include <functional>

#include "simulator_runner/Simulation.hpp"
#include "utils/types.h"

class SimulationStepper2D {
    scalar delta_time;
    scalar total_duration;
    scalar angular_momentum;
    unsigned short record_interval;

    struct motion_step_2d step;

    FILE *file_bin;
    bool isRunning = false;
    bool isFinished = false;
    bool isStopped = false;

    std::function<void()> onCompletion;

    size_t it = 0;

  public:
    SimulationStepper2D(Simulation &simulation,
                        std::function<void()> &&onCompletion, FILE *file_bin);

    ~SimulationStepper2D();

    void run();

    void pause();

    void resume();

    void stop();
};

#endif // SIMULATION_2D_STEPPER_HPP
