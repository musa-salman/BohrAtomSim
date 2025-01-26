#ifndef SPHERICAL_SIM_HPP
#define SPHERICAL_SIM_HPP

#include "atom/atom_bohr_sim.h"
#include <memory>

void simulate_spherical_orbit(std::shared_ptr<sim_ctx> ctx,
                              struct electron_orbit orbit);

#endif // SPHERICAL_SIM_HPP