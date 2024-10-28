#include "simulation_runner/simulation_runner.h"
#include "atom/atom_bohr_sim.h"
#include "utils/types.h"

#include "polar/polar_sim.h"
#include "polar/polar_sim_rel.h"
#include "spherical/spherical_sim.h"
#include "spherical/spherical_sim_rel.h"

void init_simulation(struct simulator *sim, struct sim_ctx *ctx,
                     enum sim_type type) {
    sim->ctx = ctx;

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
    struct iter_ctx *iter_ctx = sim->ctx->iter_ctx;
    struct electron_orbit *electrons = sim->ctx->atom->electrons;
    const unsigned char electron_count = sim->ctx->atom->electrons_count;

    for (iter_ctx->orbit_i = 0; iter_ctx->orbit_i < electron_count;
         iter_ctx->orbit_i++) {

        iter_ctx->electron_orbit = electrons + iter_ctx->orbit_i;

        sim->simulate_orbit(sim->ctx);
    }
}