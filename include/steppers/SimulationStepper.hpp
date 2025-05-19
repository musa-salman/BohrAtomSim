#ifndef SIMULATION_STEPPER_HPP
#define SIMULATION_STEPPER_HPP

#include <any>
#include <cstdio>
#include <functional>
#include <string>

#include "utils/types.h"

enum class ParameterType {
    Double,
    Int,
    String,
    Bool,
};

using ParameterValue = std::any;

struct ParameterSpec {
    std::string name;
    std::string label;
    ParameterType type;
    ParameterValue defaultValue;
};

enum class StepperType { Stepper2D, SymbolicPotential };

class SimulationStepper {
  protected:
    FILE *file_bin;
    bool isRunning = false;
    bool isFinished = false;
    bool isStopped = false;

    std::function<void()> onCompletion;

    size_t it = 0;

  public:
    virtual ~SimulationStepper();

    virtual const std::vector<ParameterSpec> &requiredParameters() const {
        static const std::vector<ParameterSpec> empty;
        return empty;
    }

    virtual const std::string &getName() const = 0;

    virtual void run() = 0;

    void pause();

    void resume();

    void stop();
};

#endif // SIMULATION_2D_STEPPER_HPP