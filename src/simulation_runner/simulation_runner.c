// #include <omp.h>
#include <stdio.h>

#include "atom/atom_bohr_sim.h"
#include "simulation_runner/simulation_runner.h"

#include "polar/polar_sim.h"
#include "polar/polar_sim_rel.h"
#include "spherical/spherical_sim.h"
#include "spherical/spherical_sim_rel.h"
#include "utils/iterator.h"

void init_simulation(struct simulator *sim, struct atom atom,
                     struct sim_ctx *ctx, enum sim_type type) {
    sim->ctx = ctx;
    sim->atom = atom;

    switch (type) {
    case POLAR:
        sim->simulate_orbit = simulate_polar_orbit;
        break;
    case REL_POLAR:
        sim->simulate_orbit = simulate_polar_orbit_rel;
        break;
    case SPHERICAL:
        sim->simulate_orbit = simulate_spherical_orbit;
        break;
    case REL_SPHERICAL:
        sim->simulate_orbit = simulate_spherical_rel_orbit;
        break;
    default:
        sim->simulate_orbit = NULL;
        break;
    }
}

void run_simulation(struct simulator *sim) {
    const struct electron_orbit *electrons = sim->atom.electrons;
    const unsigned char electron_count = sim->atom.electrons_count;

#pragma omp parallel for schedule(dynamic)
    for (unsigned char i = 0; i < electron_count; i++) {
        sim->simulate_orbit(sim->ctx, electrons[i]);
    }
}