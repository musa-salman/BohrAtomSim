#include <stddef.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"
#include "polar/polar_calc.h"
#include "polar/polar_calc_rel.h"
#include "polar/polar_sim_rel.h"
#include "utils/constants.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

void simulate_orbit_rel(struct sim_ctx *ctx);
bool simulate_orbit_rel_step(struct sim_ctx *ctx, long double curr_l,
                             long double Hbar_sqr, bool *is_maximum,
                             long double *prev_max_vec);

void polar_sim_rel_ele(struct sim_ctx *ctx) {
    ctx->iter_ctx->orbit_i = 0;
    for (; ctx->iter_ctx->orbit_i < ctx->atom->electrons_count;
         ctx->iter_ctx->orbit_i++) {
        ctx->iter_ctx->electron_orbit =
            ctx->atom->electrons + ctx->iter_ctx->orbit_i;
        simulate_orbit_rel(ctx);
    }
}

bool simulate_orbit_rel_step(struct sim_ctx *ctx, long double curr_l,
                             long double Hbar_sqr, bool *is_maximum,
                             long double *prev_max_vec) {
    const struct atom *atom = ctx->atom;
    struct sim_itr *curr_itr = ctx->iter_ctx->curr_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;
    long double K = ctx->iter_ctx->electron_orbit->angular;
    long double K_sqr = K * K;
    const bool is_at_interest =
        iterate(ctx->iter_ctx, ctx->time_interval, POLAR);

    next_itr->r_dot_dot = calc_rel_r_dot_dot(
        K_sqr * Hbar_sqr, atom->electron_mass, curr_itr->gamma, curr_itr->r,
        atom->electron_charge, curr_itr->r_dot);

    next_itr->phi_dot = calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->r,
                                         atom->electron_mass);

    if (!is_at_interest)
        return false;
    *is_maximum = !(*is_maximum);
    if (*is_maximum) {
        if (*prev_max_vec != 0) {
            curr_itr->delta_phi += PHI(curr_itr) - *prev_max_vec;
            if (ctx->delta_psi_mode) {
                RECORD_ITERATION(ctx, curr_itr);
            }
            next_itr->delta_phi = DELTA_PHI(curr_itr);
        }

        *prev_max_vec = PHI(curr_itr);
    }

    next_itr->gamma = calc_rel_gamma(curr_l, atom->electron_mass, R(curr_itr),
                                     R_DOT(curr_itr));
    return is_at_interest;
}

void simulate_orbit_rel(struct sim_ctx *ctx) {
    const struct atom *atom = ctx->atom;
    struct sim_itr *curr_itr = ctx->iter_ctx->curr_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;
    const struct electron_orbit *electron_orbit = ctx->iter_ctx->electron_orbit;

    double N = electron_orbit->principal;
    double K = electron_orbit->angular;

    // The multiplier of H_BAR
    struct radial_bounds *radial_bounds = compute_radial_limits(N, K);
    // L is the value of H_Bar
    long double curr_l = H_BAR * K;
    long double K_sqr = K * K;

    long double prev_max_vec = 0;

    init_iteration(curr_itr, REL_POLAR);
    init_iteration(next_itr, REL_POLAR);

    curr_itr->r = radial_bounds->r_min;

    curr_itr->gamma = calc_rel_gamma(curr_l, atom->electron_mass, curr_itr->r,
                                     curr_itr->r_dot);

    curr_itr->r_dot_dot = calc_rel_r_dot_dot(
        K_sqr * H_BAR_SQR, atom->electron_mass, curr_itr->gamma, curr_itr->r,
        atom->electron_charge, curr_itr->r_dot);

    curr_itr->phi_dot =
        calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->r, MASS(atom));
    long double revolutions = ctx->revolutions;

    bool at_max = true;
    bool is_at_interest = false;
    for (unsigned long it = 1; it < ctx->max_iters; it++) {
        is_at_interest = simulate_orbit_rel_step(ctx, curr_l, H_BAR_SQR,
                                                 &at_max, &prev_max_vec);

        if (it % ctx->record_interval == 0 && !(ctx->delta_psi_mode)) {
            RECORD_ITERATION(ctx, curr_itr);
        }

        if (is_at_interest) {
            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        struct sim_itr *temp = curr_itr;
        ctx->iter_ctx->curr_itr = curr_itr;
        ctx->iter_ctx->next_itr = temp;
    }

    free(radial_bounds);
    end_iteration(ctx->iter_ctx);
    RECORD_ITERATION(ctx, curr_itr);
}