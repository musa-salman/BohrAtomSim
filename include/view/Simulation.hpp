#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "atom/atom_bohr_sim.h"

struct Simulation {
    Simulation(const std::string &name, int type,
               const std::vector<std::string> &fields);

    Simulation &operator=(const Simulation &other);

    void appendData(const std::unordered_map<std::string, scalar> &data) const;

    std::shared_ptr<std::unordered_map<std::string, std::vector<scalar>>>
    getData() const;

    std::shared_ptr<std::unordered_map<std::string, std::vector<scalar>>> data;
    int type;
    char name[256];
    struct electron_orbit orbit;

    bool delta_psi_mode;
    float revolutions;
    double time_interval;
    unsigned short record_interval;
};

#endif // SIMULATION_HPP