#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"
#include "utils/iterator.h"

struct Simulation {
    std::string name;
    size_t id;

    unsigned short record_interval;
    double delta_time;
    double total_duration;

    double r_0;
    double v_0;
    double theta_rv;

    struct motion_step_2d initial_motion_step;

    enum class SimulationStatus { IDLE, RUNNING, PAUSED, COMPLETED };

    SimulationStatus status = SimulationStatus::IDLE;

    Simulation(const std::string &name, unsigned short record_interval,
               float total_duration, double time_interval, double r_0,
               double v_0, double theta_rv)
        : name(name), record_interval(record_interval),
          delta_time(time_interval), total_duration(total_duration), r_0(r_0),
          v_0(v_0), theta_rv(theta_rv) {

        if (name.empty()) {
            throw std::invalid_argument("name cannot be empty");
        }

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }

    Simulation(const std::string &name, size_t id,
               unsigned short record_interval, float total_duration,
               double time_interval, double r_0, double v_0, double theta_rv)
        : name(name), id(id), record_interval(record_interval),
          delta_time(time_interval), total_duration(total_duration), r_0(r_0),
          v_0(v_0), theta_rv(theta_rv) {

        if (name.empty()) {
            throw std::invalid_argument("name cannot be empty");
        }

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }

    Simulation()
        : name(""), record_interval(10000), delta_time(1e-7),
          total_duration(800.0), r_0(2.2917960675006309107), v_0(0.87267799625),
          theta_rv(HALF_PI) {
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }
};

#endif // SIMULATION_HPP