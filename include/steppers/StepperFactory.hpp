#ifndef STEPPER_FACTORY_HPP
#define STEPPER_FACTORY_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "steppers/SimulationStepper.hpp"

class StepperFactory {
  public:
    using CreatorFunc = std::function<std::unique_ptr<SimulationStepper>(
        const std::unordered_map<std::string, ParameterValue> &params,
        std::function<void()> &&onCompletion, FILE *file_bin)>;

    void registerStepper(StepperType type, CreatorFunc func);

    std::unique_ptr<SimulationStepper>
    create(StepperType type,
           const std::unordered_map<std::string, ParameterValue> &params,
           std::function<void()> &&onCompletion, FILE *file_bin) const;

    std::vector<std::string> availableSteppers() const;

  private:
    std::unordered_map<StepperType, CreatorFunc> creators;
};

#endif // STEPPER_FACTORY_HPP
