#ifdef __cplusplus
extern "C" {
#endif

#ifndef SPHERICAL_SIM_H
#define SPHERICAL_SIM_H

#include "atom/atom_bohr_sim.h"

void simulate_spherical_orbit(struct sim_ctx *ctx, struct electron_orbit orbit);

#endif // SPHERICAL_SIM_H

#ifdef __cplusplus
}
#endif