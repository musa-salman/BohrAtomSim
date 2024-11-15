#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

#include "atom/atom_bohr_sim.h"
#include "utils/iterator.h"

typedef void (*simulate_orbit_fn)(struct sim_ctx*, struct electron_orbit);

struct simulator {
    simulate_orbit_fn simulate_orbit;
    struct atom atom;
    struct sim_ctx* ctx;
};

void init_simulation(struct simulator* sim, struct atom atom, struct sim_ctx* ctx, enum sim_type type);

void run_simulation(struct simulator* sim);

#endif // SIMULATION_RUNNER_H