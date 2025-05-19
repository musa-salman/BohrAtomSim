#ifndef SIMULATION_STEPPER_2D_HPP
#define SIMULATION_STEPPER_2D_HPP

#include <cstdio>
#include <functional>
#include <unordered_map>

#include "simulator_runner/Simulation.hpp"
#include "steppers/SimulationStepper.hpp"
#include "utils/types.h"

class SimulationStepper2D : public SimulationStepper {
    scalar delta_time;
    scalar total_duration;
    scalar angular_momentum;
    int record_interval;

    struct motion_step_2d step;

  public:
    SimulationStepper2D(
        const std::unordered_map<std::string, ParameterValue> &params,
        std::function<void()> &&onCompletion, FILE *file_bin);

    void run() override;

    const std::string &getName() const override {
        static const std::string name = "Simulation Stepper 2D";
        return name;
    }

    void pause();

    void resume();

    void stop();

    const std::vector<ParameterSpec> &requiredParameters() const override;

  private:
    static const std::vector<ParameterSpec> &spec();
};

#endif // SIMULATION_2D_STEPPER_HPP
