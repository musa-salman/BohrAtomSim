#include "steppers/StepperFactory.hpp"

void StepperFactory::registerStepper(StepperType type, CreatorFunc func) {
    creators[type] = std::move(func);
}

std::unique_ptr<SimulationStepper> StepperFactory::create(
    StepperType type,
    const std::unordered_map<std::string, ParameterValue> &params,
    std::function<void()> &&onCompletion, FILE *file_bin) const {
    return creators.at(type)(params, std::move(onCompletion), file_bin);
}

std::vector<std::string> StepperFactory::availableSteppers() const {
    std::vector<std::string> stepperNames;
    for (const auto &pair : creators) {
        stepperNames.push_back(std::to_string(static_cast<int>(pair.first)));
    }
    return stepperNames;
}