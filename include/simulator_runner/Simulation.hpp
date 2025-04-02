#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#include "atom/atom_bohr_sim.h"
#include "utils/iterator.h"

struct Simulation {
    std::string name;
    electron_orbit orbit;

    size_t id;

    int type;
    unsigned short record_interval;
    float revolutions;
    double time_interval;

    Simulation(const std::string &_name, const electron_orbit _orbit, int type,
               unsigned short record_interval, float revolutions,
               double time_interval)
        : name(_name), type(type), record_interval(record_interval),
          revolutions(revolutions), time_interval(time_interval) {
        this->orbit.principal = _orbit.principal;
        this->orbit.angular = _orbit.angular;
        this->orbit.magnetic = _orbit.magnetic;

        if (name.empty()) {
            throw std::invalid_argument("Name cannot be empty");
        }
    }

    Simulation(const std::string &_name, size_t _id,
               const electron_orbit _orbit, int type,
               unsigned short record_interval, float revolutions,
               double time_interval)
        : name(_name), id(_id), type(type), record_interval(record_interval),
          revolutions(revolutions), time_interval(time_interval) {
        this->orbit.principal = _orbit.principal;
        this->orbit.angular = _orbit.angular;
        this->orbit.magnetic = _orbit.magnetic;

        if (name.empty()) {
            throw std::invalid_argument("Name cannot be empty");
        }
    }

    Simulation()
        : name(""), orbit({3, 2, 1, 1}), type(POLAR), record_interval(1000),
          revolutions(10), time_interval(1e-6) {}

    inline const char *getType() const {
        switch (type) {
        case POLAR:
            return "2DNR";
        case SPHERICAL:
            return "3DNR";
        case REL_POLAR:
            return "2DR";
        case REL_SPHERICAL:
            return "3DR";
        case SPIN:
            return "3DRS";
        default:
            return "Unknown";
        }
    }
};

#endif // SIMULATION_HPP