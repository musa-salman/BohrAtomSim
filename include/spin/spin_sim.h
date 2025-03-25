#ifdef __cplusplus
extern "C" {
#endif

#ifndef SPIN_SIM_H
#define SPIN_SIM_H

#include "atom/atom_bohr_sim.h"

void simulate_spin_orbit(const struct sim_ctx ctx);

#endif // SPIN_SIM_H

#ifdef __cplusplus
}
#endif