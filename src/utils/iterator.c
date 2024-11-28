#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

void start_iteration(struct iter_ctx *ctx) { ctx->start_time = clock(); }

void end_iteration(struct iter_ctx *ctx) { ctx->end_time = clock(); }

void init_iteration(struct sim_itr *itr, enum sim_type type) {
    *itr = (struct sim_itr){.dt = 0,
                            .r = 0,
                            .r_dot = 0,
                            .r_dot_dot = 0,
                            .phi = 0,
                            .phi_dot = 0,
                            // REL
                            .gamma = -1,
                            .delta_phi = -1,
                            // 3D
                            .theta = -1,
                            .theta_dot = -1,
                            .theta_dot_dot = -1,
                            // spin
                            .epsilon = -1,
                            .initial_phi_dot = -1};

    if (type == REL_POLAR || type == REL_SPHERICAL || type == REL_SPIN) {
        itr->gamma = 0;
        itr->delta_phi = 0;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        itr->theta = 0;
        itr->theta_dot = 0;
        itr->theta_dot_dot = 0;
    }

    if (type == SPIN || type == REL_SPIN) {
        itr->epsilon = 0;
        itr->initial_phi_dot = 0;
    }
}

bool iterate(struct iter_ctx *ctx, scalar time_interval, enum sim_type type) {
    const struct sim_itr *curr_itr = ctx->prev_itr;
    struct sim_itr *next_itr = ctx->next_itr;

    next_itr->dt += time_interval;

    next_itr->r_dot = R_DOT(curr_itr) + R_DOT_DOT(curr_itr) * time_interval;
    next_itr->r = R(curr_itr) + R_DOT(curr_itr) * time_interval;

    next_itr->phi = PHI(curr_itr) + PHI_DOT(curr_itr) * time_interval;

    if (PHI(next_itr) > TWO_PI) {
        next_itr->phi -= TWO_PI;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        next_itr->theta_dot =
            THETA_DOT(curr_itr) + THETA_DOT_DOT(curr_itr) * time_interval;
        next_itr->theta = THETA(curr_itr) + THETA_DOT(curr_itr) * time_interval;
    }

    return R_DOT(next_itr) * R_DOT(curr_itr) < 0;
}