#include <stddef.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "polar/polar_calc.h"
#include "polar/polar_sim.h"

#include "utils/constants.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct sim_ctx *ctx, long double curr_l,
                                bool *is_maximum, long double *prev_max_vec,
                                long double *prevR);

void simulate_polar_orbit(struct sim_ctx *ctx) {
    struct iter_ctx *iter_ctx = ctx->iter_ctx;
    start_iteration(iter_ctx);
    struct sim_itr *curr_itr = iter_ctx->curr_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;
    const struct atom *atom = ctx->atom;

    double N = iter_ctx->electron_orbit->principal;
    double k = iter_ctx->electron_orbit->angular;

    struct radial_bounds *radial_bounds = compute_radial_limits(N, k);
    long double curr_l = H_BAR * k;

    long double prev_max_vec = 0;
    long double prevR = 0;

    init_iteration(curr_itr, POLAR);
    init_iteration(next_itr, POLAR);

    curr_itr->r = radial_bounds->r_min;
    curr_itr->r_dot_dot =
        compute_r_dot_dot(MASS(atom), curr_itr->r, CHARGE(atom), k);
    curr_itr->phi_dot = compute_phi_dot(curr_l, MASS(atom), curr_itr->r);

    long double revolutions = ctx->revolutions;

    bool is_maximum = true;

    for (unsigned long it = 1; it < ctx->max_iters; it++) {

        const bool is_at_interest = simulate_orbit_step(
            ctx, curr_l, &is_maximum, &prev_max_vec, &prevR);

        if (it % ctx->record_interval == 0 && !ctx->delta_psi_mode) {
            RECORD_ITERATION(ctx, curr_itr);
        }

        if (is_at_interest) {
            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        struct sim_itr *temp = curr_itr;
        iter_ctx->curr_itr = next_itr;
        iter_ctx->next_itr = temp;
    }

    RECORD_ITERATION(ctx, curr_itr);
    end_iteration(iter_ctx);
    free(radial_bounds);
}

static bool simulate_orbit_step(struct sim_ctx *ctx, long double curr_l,
                                bool *is_maximum, long double *prev_max_vec,
                                long double *prev_r) {
    struct sim_itr *curr_itr = ctx->iter_ctx->curr_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;
    long double k = ctx->iter_ctx->electron_orbit->angular;

    const bool is_at_interest =
        iterate(ctx->iter_ctx, ctx->time_interval, POLAR);

    next_itr->r_dot_dot = compute_r_dot_dot(
        ctx->atom->electron_mass, curr_itr->r, ctx->atom->electron_charge, k);
    next_itr->phi_dot =
        compute_phi_dot(curr_l, ctx->atom->electron_mass, curr_itr->r);

    if (!is_at_interest)
        return false;

    *is_maximum = !(*is_maximum);
    if (!(*is_maximum))
        return true;

    if (*prev_max_vec != 0) {
        curr_itr->delta_phi += curr_itr->phi - *prev_max_vec;

        if (ctx->delta_psi_mode) {
            RECORD_ITERATION(ctx, curr_itr);
        }

        next_itr->delta_phi = curr_itr->delta_phi;
    }

    *prev_r = R(curr_itr);
    *prev_max_vec = PHI(curr_itr);

    return true;
}