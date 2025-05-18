#include "simulation_2d/SimulationStepper2D.hpp"
#include <cstdio>
#include <thread>

#ifdef _WIN32
#include <io.h>
#define fileno _fileno
#define fsync _commit
#else
#include <unistd.h>
#endif

inline static void simulate_step(struct motion_step_2d *step, scalar delta_time,
                                 scalar angular_momentum);

SimulationStepper2D::SimulationStepper2D(Simulation &simulation,
                                         std::function<void()> &&onCompletion,
                                         FILE *file_bin)
    : delta_time(simulation.getDeltaTime()),
      total_duration(simulation.getTotalDuration()),
      angular_momentum(simulation.getR0() * simulation.getV0() *
                       sin(simulation.getThetaRV())),
      record_interval(simulation.getRecordInterval()),
      step(simulation.initial_motion_step), file_bin(file_bin),
      onCompletion(onCompletion) {
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
    isRunning = true;

    while (step.time < total_duration) {
        simulate_step(&step, delta_time, angular_momentum);
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

void SimulationStepper2D::pause() {
    if (isRunning)
        isRunning = false;
}

void SimulationStepper2D::resume() {
    if (!isRunning) {
        isRunning = true;
        run();
    }
}

void SimulationStepper2D::stop() {
    if (isRunning) {
        isRunning = false;
        isStopped = true;

        while (!isFinished)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

inline static void simulate_step(struct motion_step_2d *step, scalar delta_time,
                                 scalar angular_momentum) {
    step->time = step->time + delta_time;
    step->r_dot = step->r_dot + step->r_ddot * delta_time;
    step->r = step->r + step->r_dot * delta_time;

    step->psi = step->psi + step->psi_dot * delta_time;

    if (step->psi > TWO_PI)
        step->psi -= TWO_PI;

    step->r_ddot =
        compute_rel_r_ddot(angular_momentum, step->gamma, step->r, step->r_dot);

    step->psi_dot = POLAR_PHI_DOT_REL(angular_momentum, step->r, step->gamma);

    step->gamma = compute_gamma(angular_momentum, step->r, step->r_dot);
}