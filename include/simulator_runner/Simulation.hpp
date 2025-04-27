#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

struct Simulation {
    std::string name;
    size_t id;

    unsigned short record_interval;
    double time_interval;
    double total_duration;

    double r_0;
    double v_0;
    double theta_rv;

    Simulation(const std::string &name, unsigned short record_interval,
               float total_duration, double time_interval, double r_0,
               double v_0, double theta_rv)
        : name(name), record_interval(record_interval),
          time_interval(time_interval), total_duration(total_duration),
          r_0(r_0), v_0(v_0), theta_rv(theta_rv) {

        if (name.empty()) {
            throw std::invalid_argument("name cannot be empty");
        }
    }

    Simulation(const std::string &name, size_t id,
               unsigned short record_interval, float total_duration,
               double time_interval, double r_0, double v_0, double theta_rv)
        : name(name), id(id), record_interval(record_interval),
          time_interval(time_interval), total_duration(total_duration),
          r_0(r_0), v_0(v_0), theta_rv(theta_rv) {

        if (name.empty()) {
            throw std::invalid_argument("name cannot be empty");
        }
    }

    Simulation()
        : name(""), record_interval(1000), time_interval(0.01),
          total_duration(10.0), r_0(2), v_0(1), theta_rv(HALF_PI) {}
};

#endif // SIMULATION_HPP