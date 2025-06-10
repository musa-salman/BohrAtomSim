#pragma once

#include "SimulationStepper.hpp"
#include "physics/math/macros.hpp"
#include "storage/format/SimulationHeaderWriter.hpp"

namespace physics::stepper {
template <typename State>
class SimulationStepperImpl : public SimulationStepper {
  public:
    SimulationStepperImpl(const StepperCommonConfig &cfg, State &&state)
        : SimulationStepper(cfg), step(std::forward<State>(state)) {
        storage::format::writeSimulationHeader(file_bin,
                                               std::span{State::field_names});
    }

    void executeSimulationLoop() override {
        while (step.time < total_duration) [[likely]] {
            const bool isLocalMaxR = step.iterate(delta_time);
            rLocalMaxCount += static_cast<int>(isLocalMaxR);
            if (it % record_interval == 0 ||
                static_cast<int>(isLocalMaxR) * rLocalMaxCount > 1)
                [[unlikely]] {
                recordStep();
                if (!isRunning || ((rLocalMaxCount >= rLocalMaxLimit) &&
                                   (rLocalMaxLimit > 0)))
                    break;
            }
            it++;
        }
    }

  private:
    State step;

    SIM_INLINE void recordStep() const noexcept {
        const auto data = step.to_array();
        fwrite(data.data(), sizeof(double), data.size(), file_bin);
    }
};
} // namespace physics::stepper