#ifndef SYMBOLIC_POTENTIAL_STEPPER_HPP
#define SYMBOLIC_POTENTIAL_STEPPER_HPP

#include <cstdio>
#include <exprtk.hpp>
#include <functional>
#include <unordered_map>

#include "simulator_runner/Simulation.hpp"
#include "steppers/SimulationStepper.hpp"
#include "utils/types.h"

class SymbolicPotentialStepper : public SimulationStepper {
    scalar delta_time;
    scalar total_duration;
    scalar angular_momentum;
    int record_interval;
    exprtk::expression<scalar> du_dr;

    struct motion_step_2d step;

  public:
    SymbolicPotentialStepper(
        const std::unordered_map<std::string, ParameterValue> &params,
        std::function<void()> &&onCompletion, FILE *file_bin);

    void run() override;

    const std::string &getName() const override {
        static const std::string name = "Generic Simulation Stepper 2D";
        return name;
    }

    void pause();

    void resume();

    void stop();

    const std::vector<ParameterSpec> &requiredParameters() const override;

  private:
    static const std::vector<ParameterSpec> &spec();
};

#endif // SYMBOLIC_POTENTIAL_STEPPER_HPP
