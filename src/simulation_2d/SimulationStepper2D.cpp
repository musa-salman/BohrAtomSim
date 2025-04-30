#include "simulation_2d/SimulationStepper2D.hpp"
#include <thread>

inline static void simulate_step(struct motion_step_2d *step, scalar delta_time,
                                 scalar angular_momentum);

SimulationStepper2D::SimulationStepper2D(Simulation &simulation,
                                         std::function<void()> onCompletion,
                                         FILE *file_bin)
    : delta_time(simulation.delta_time),
      total_duration(simulation.total_duration),
      angular_momentum(simulation.r_0 * simulation.v_0 *
                       sin(simulation.theta_rv)),
      record_interval(simulation.record_interval),
      step(simulation.initial_motion_step), file_bin(file_bin),
      status(&simulation.status), onCompletion(onCompletion) {
    if (!file_bin) {
        throw std::runtime_error("Failed to open file for writing");
    }
}

SimulationStepper2D::~SimulationStepper2D() {
    if (file_bin) {
        fclose(file_bin);
        file_bin = nullptr;
    }
}

void SimulationStepper2D::run() {
    *status = Simulation::SimulationStatus::RUNNING;
    isRunning = true;

    while (step.time < total_duration) {
        simulate_step(&step, delta_time, angular_momentum);
        if (it % record_interval == 0) {
            const double polar_values[7] = {C2D(step.time),  C2D(step.r),
                                            C2D(step.r_dot), C2D(step.r_ddot),
                                            C2D(step.phi),   C2D(step.phi_dot),
                                            C2D(step.gamma)};
            fwrite(polar_values, sizeof(double), 7, file_bin);
            if (!isRunning)
                break;
        }

        it++;
    }

    if (step.time >= total_duration ||
        *status == Simulation::SimulationStatus::COMPLETED) {
        if (file_bin) {
            fclose(file_bin);
            file_bin = nullptr;
        }
        onCompletion();
        isFinished = true;
    }
}

void SimulationStepper2D::pause() {
    if (isRunning) {
        isRunning = false;
        *status = Simulation::SimulationStatus::PAUSED;
    }
}

void SimulationStepper2D::resume() {
    if (!isRunning) {
        isRunning = true;
        *status = Simulation::SimulationStatus::RUNNING;
        run();
    }
}

void SimulationStepper2D::stop() {
    if (isRunning) {
        isRunning = false;
        *status = Simulation::SimulationStatus::COMPLETED;

        while (!isFinished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

inline static void simulate_step(struct motion_step_2d *step, scalar delta_time,
                                 scalar angular_momentum) {
    step->time = step->time + delta_time;
    step->r_dot = step->r_dot + step->r_ddot * delta_time;
    step->r = step->r + step->r_dot * delta_time;

    step->phi = step->phi + step->phi_dot * delta_time;

    if (step->phi > TWO_PI) {
        step->phi -= TWO_PI;
    }

    step->r_ddot =
        compute_rel_r_ddot(angular_momentum, step->gamma, step->r, step->r_dot);

    step->phi_dot = POLAR_PHI_DOT_REL(angular_momentum, step->r, step->gamma);

    step->gamma = compute_gamma(angular_momentum, step->r, step->r_dot);
}