#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIMULATION_2D_H
#define SIMULATION_2D_H

#include "atom/atom_bohr_sim.h"

void simulate_2d_electron_motion(const struct sim2d_ctx ctx);

#endif // SIMULATION_2D_H

#ifdef __cplusplus
}
#endif