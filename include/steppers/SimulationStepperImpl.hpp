#ifndef SIMULATION_STEPPER_IMPL_HPP
#define SIMULATION_STEPPER_IMPL_HPP

#include "atom/result_recorders.hpp"
#include "eom/utils.hpp"
#include "steppers/SimulationStepper.hpp"

template <typename State>
class SimulationStepperImpl : public SimulationStepper {
  public:
    SimulationStepperImpl(const StepperCommonConfig &cfg, State &&state)
        : SimulationStepper(cfg), step(std::forward<State>(state)) {
        const auto &field_names = State::field_names;
        init_file_header(file_bin, std::span{field_names});
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

    SIM_INLINE inline void recordStep() const noexcept {
        const auto data = step.to_array();
        fwrite(data.data(), sizeof(double), data.size(), file_bin);
    }
};

#endif