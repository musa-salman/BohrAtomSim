#ifndef STEPPER_FACTORY_HPP
#define STEPPER_FACTORY_HPP

#include <memory>

#include "steppers/SimulationStepper.hpp"
#include "steppers/SimulationStepperImpl.hpp"

class StepperFactory {
  public:
    template <typename State>
    std::unique_ptr<SimulationStepper>
    create(State &&state, const StepperCommonConfig &config) const {
        return std::make_unique<SimulationStepperImpl<std::decay_t<State>>>(
            config, std::forward<State>(state));
    }
};

#endif // STEPPER_FACTORY_HPP
