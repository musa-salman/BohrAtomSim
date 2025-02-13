#ifndef SIMULATION_FIELDS_HPP
#define SIMULATION_FIELDS_HPP

#include <string>
#include <vector>

#include "utils/iterator.h"

// Fixed fields for each simulation type
const std::vector<std::string> SIMULATION_2D_NON_REL = {
    "t", "r", "r_dot", "r_dot_dot", "psi", "psi_dot"};

const std::vector<std::string> SIMULATION_2D_REL = {
    "t", "r", "r_dot", "r_dot_dot", "psi", "psi_dot", "gamma", "delta_psi"};

const std::vector<std::string> SIMULATION_3D_NON_REL = {
    "t",       "r",     "r_dot",     "r_dot_dot",    "phi",
    "phi_dot", "theta", "theta_dot", "theta_dot_dot"};

const std::vector<std::string> SIMULATION_3D_REL = {
    "t",     "r",         "r_dot",         "r_dot_dot", "phi",      "phi_dot",
    "theta", "theta_dot", "theta_dot_dot", "gamma",     "delta_psi"};

inline std::vector<std::string> getColumnNames(int type) {
    switch (type) {
    case POLAR:
        return SIMULATION_2D_NON_REL;
    case REL_POLAR:
        return SIMULATION_2D_REL;
    case SPHERICAL:
        return SIMULATION_3D_NON_REL;
    case REL_SPHERICAL:
        return SIMULATION_3D_REL;
    default:
        return {};
    }
}

#endif // SIMULATION_FIELDS_HPP
