#ifndef SIMULATION_STEPPER_IMPL_HPP
#define SIMULATION_STEPPER_IMPL_HPP

#include "atom/result_recorders.hpp"
#include "eom/utils.hpp"
#include "steppers/SimulationStepper.hpp"

template <typename State>
class SimulationStepperImpl : public SimulationStepper {
  public:
    SimulationStepperImpl(const StepperCommonConfig &cfg, State &&state)
        : SimulationStepper(cfg), step(std::move(state)) {
        const auto &field_names = State::field_names;
        init_file_header(file_bin, std::span{field_names});
    }

    void executeSimulationLoop() override {
        while (step.time < total_duration) [[likely]] {
            const bool mustRecord = step.iterate(delta_time);
            if (it % record_interval == 0 || mustRecord) [[unlikely]] {
                recordStep();
                if (!isRunning)
                    break;
            }
            it++;
        }
    }

  private:
    State step;

    SIM_INLINE inline void recordStep() {
        const auto data = step.to_array();
        fwrite(data.data(), sizeof(double), data.size(), file_bin);
    }
};

#endif