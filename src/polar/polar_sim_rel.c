#include <stddef.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"

#include "polar/polar_sim_rel.h"

#include "orbital_math.h"
#include "utils/constants.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_rel_step(struct sim_ctx *ctx, long double curr_l,
                                    bool *is_maximum,
                                    long double *prev_max_vec);

void simulate_polar_orbit_rel(struct sim_ctx *ctx) {
    const struct atom *atom = ctx->atom;
    struct sim_itr *curr_itr = ctx->iter_ctx->curr_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;
    const struct electron_orbit *electron_orbit = ctx->iter_ctx->electron_orbit;

    double N = electron_orbit->principal;
    double K = electron_orbit->angular;

    struct radial_bounds *radial_bounds = compute_radial_limits(N, K);

    long double curr_l = H_BAR * K;

    long double prev_max_vec = 0;

    init_iteration(curr_itr, REL_POLAR);
    init_iteration(next_itr, REL_POLAR);

    curr_itr->r = radial_bounds->r_min;

    curr_itr->gamma = compute_rel_gamma(curr_l, atom->electron_mass,
                                        curr_itr->r, curr_itr->r_dot);

    curr_itr->r_dot_dot =
        compute_rel_r_dot_dot(K, MASS(atom), GAMMA(curr_itr), R(curr_itr),
                              CHARGE(atom), R_DOT(curr_itr));

    curr_itr->phi_dot =
        POLAR_PHI_DOT_REL(K, MASS(atom), R(curr_itr), curr_itr->gamma);
    long double revolutions = ctx->revolutions;

    bool at_max = true;
    bool is_at_interest = false;
    for (unsigned long it = 1; it < ctx->max_iters; it++) {
        is_at_interest =
            simulate_orbit_rel_step(ctx, curr_l, &at_max, &prev_max_vec);

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

static bool simulate_orbit_rel_step(struct sim_ctx *ctx, long double curr_l,
                                    bool *is_maximum,
                                    long double *prev_max_vec) {
    const struct atom *atom = ctx->atom;
    struct sim_itr *curr_itr = ctx->iter_ctx->curr_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;
    long double K = ctx->iter_ctx->electron_orbit->angular;

    const bool is_at_interest =
        iterate(ctx->iter_ctx, ctx->time_interval, POLAR);

    next_itr->r_dot_dot =
        compute_rel_r_dot_dot(K, MASS(atom), GAMMA(curr_itr), R(curr_itr),
                              CHARGE(atom), R_DOT(curr_itr));

    next_itr->phi_dot =
        POLAR_PHI_DOT_REL(K, MASS(atom), R(curr_itr), curr_itr->gamma);

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

    next_itr->gamma = compute_rel_gamma(curr_l, atom->electron_mass,
                                        R(curr_itr), R_DOT(curr_itr));
    return is_at_interest;
}