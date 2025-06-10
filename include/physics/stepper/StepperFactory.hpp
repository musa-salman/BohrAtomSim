#pragma once

#include <memory>

#include "SimulationStepper.hpp"
#include "SimulationStepperImpl.hpp"

namespace physics::stepper {
class StepperFactory {
  public:
    template <typename State>
    std::unique_ptr<SimulationStepper>
    create(State &&state, const StepperCommonConfig &config) const {
        return std::make_unique<SimulationStepperImpl<std::decay_t<State>>>(
            config, std::forward<State>(state));
    }
};
} // namespace physics::stepper
