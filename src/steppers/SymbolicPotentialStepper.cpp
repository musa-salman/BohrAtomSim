#include <any>
#include <cstdio>
#ifdef _WIN32
#include <io.h>
#define fileno _fileno
#define fsync _commit
#else
#include <unistd.h>
#endif

#include "atom/result_recorders.h"
#include "math_utils.hpp"
#include "steppers/SymbolicPotentialStepper.hpp"

inline static void simulate_step(struct motion_step_2d *step, scalar delta_time,
                                 scalar angular_momentum, scalar du_dr);

SymbolicPotentialStepper::SymbolicPotentialStepper(
    const std::unordered_map<std::string, ParameterValue> &params,
    std::function<void()> &&onCompletion, FILE *file_bin) {
    if (params.empty()) {
        this->file_bin = nullptr;
        return;
    }
    this->file_bin = file_bin;
    this->onCompletion = std::move(onCompletion);
    this->isRunning = false;
    this->isFinished = false;
    this->isStopped = false;
    this->it = 0;

    this->delta_time =
        static_cast<scalar>(std::any_cast<double>(params.at("delta_time")));

    this->total_duration =
        static_cast<scalar>(std::any_cast<double>(params.at("total_duration")));
    this->record_interval = std::any_cast<int>(params.at("record_interval"));

    {
        double initial_position[2] = {std::any_cast<double>(params.at("rx")),
                                      std::any_cast<double>(params.at("ry"))};

        double initial_velocity[2] = {
            std::any_cast<double>(params.at("vx")),
            std::any_cast<double>(params.at("vy")),
        };

        double initial_theta_rv =
            computeAngleVec2D(initial_position, initial_velocity);

        const double r0 = sqrt(initial_position[0] * initial_position[0] +
                               initial_position[1] * initial_position[1]);
        const double v0 = sqrt(initial_velocity[0] * initial_velocity[0] +
                               initial_velocity[1] * initial_velocity[1]);
        this->angular_momentum = v0 * r0 * sim_sin(initial_theta_rv);
        init_motion_step(&step, r0, v0, initial_theta_rv);
    }

    // TODO: check if already initialized
    {
        const uint8_t field_names_2DR[][MAX_FIELD_NAME] = {
            "t", "r", "r_dot", "r_ddot", "psi", "psi_dot", "gamma"};

        init_file_header(file_bin, field_names_2DR,
                         sizeof(field_names_2DR) / sizeof(field_names_2DR[0]));
    }

    exprtk::parser<scalar> parser;
    exprtk::symbol_table<scalar> symbol_table;
    symbol_table.add_variable("r", step.r);

    du_dr.register_symbol_table(symbol_table);

    parser.compile(
        std::any_cast<std::string>(params.at("potential_expression")), du_dr);
}

const std::vector<ParameterSpec> &
SymbolicPotentialStepper::requiredParameters() const {
    static const std::vector<ParameterSpec> specs = {
        {"rx", "Initial X Position", ParameterType::Double, 0.0},
        {"ry", "Initial Y Position", ParameterType::Double,
         2.2917960675006309107},
        {"vx", "Initial X Velocity", ParameterType::Double, 0.87267799625},
        {"vy", "Initial Y Velocity", ParameterType::Double, 0.0},
        {"delta_time", "Delta Time", ParameterType::Double, 1e-5},
        {"total_duration", "Total Duration", ParameterType::Double, 800.0},
        {"record_interval", "Record Interval", ParameterType::Int, 10'000},
        {
            "dU_dr(r)",
            "Potential Expression",
            ParameterType::String,
            std::string("-1/(r^2)"),
        },
    };
    return specs;
}

void SymbolicPotentialStepper::run() {
    isRunning = true;

    while (step.time < total_duration) {
        simulate_step(&step, delta_time, angular_momentum, du_dr.value());
        if (it % record_interval == 0) {
            const double polar_values[7] = {C2D(step.time),  C2D(step.r),
                                            C2D(step.r_dot), C2D(step.r_ddot),
                                            C2D(step.psi),   C2D(step.psi_dot),
                                            C2D(step.gamma)};
            fwrite(polar_values, sizeof(double), 7, file_bin);
            if (!isRunning)
                break;
        }

        it++;
    }

    if (step.time >= total_duration || isStopped) {
        if (file_bin) {
            fflush(file_bin);
            fsync(fileno(file_bin));
            fclose(file_bin);
            file_bin = nullptr;
        }
        onCompletion();
        isFinished = true;
    }
}

inline static scalar compute_r_ddot_U(scalar angular_momentum, scalar gamma,
                                      scalar r, scalar r_dot, scalar du_dr) {
    const scalar term1 =
        SQUARE(angular_momentum) / (SQUARE(gamma) * SQUARE(r) * r);
    const scalar term2 = du_dr / gamma;
    const scalar term3 = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

    const scalar result = term1 + term2 * (1 - term3);
    return result;
}

inline static void simulate_step(struct motion_step_2d *step, scalar delta_time,
                                 scalar angular_momentum, scalar du_dr) {
    step->time = step->time + delta_time;
    step->r_dot = step->r_dot + step->r_ddot * delta_time;
    step->r = step->r + step->r_dot * delta_time;

    step->psi = step->psi + step->psi_dot * delta_time;

    if (step->psi > TWO_PI)
        step->psi -= TWO_PI;

    step->r_ddot = compute_r_ddot_U(angular_momentum, step->gamma, step->r,
                                    step->r_dot, du_dr);

    step->psi_dot = POLAR_PHI_DOT_REL(angular_momentum, step->r, step->gamma);

    step->gamma = compute_gamma(angular_momentum, step->r, step->r_dot);
}