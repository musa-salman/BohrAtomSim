#include <stddef.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "polar/polar_sim.h"

#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct sim_ctx *ctx, bool *is_maximum,
                                scalar *prev_phi, scalar *prevR);

void simulate_polar_orbit(struct sim_ctx *ctx) {
    struct iter_ctx *iter_ctx = ctx->iter_ctx;
    start_iteration(iter_ctx);
    struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    quantum_principle principal = iter_ctx->electron_orbit->principal;
    quantum_angular angular = iter_ctx->electron_orbit->angular;

    struct radial_bounds radial_bounds =
        compute_radial_limits(principal, angular);

    scalar prev_phi = 0;
    scalar prevR = 0;

    init_iteration(prev_itr, POLAR);
    init_iteration(next_itr, POLAR);

    prev_itr->r = radial_bounds.r_min;
    prev_itr->r_dot_dot = compute_r_dot_dot(R(prev_itr), angular);
    prev_itr->phi_dot = POLAR_PHI_DOT(angular, R(prev_itr));

    scalar revolutions = ctx->revolutions;

    bool is_maximum = true;

    for (unsigned long it = 1; it < ctx->max_iters; it++) {

        const bool is_at_interest =
            simulate_orbit_step(ctx, &is_maximum, &prev_phi, &prevR);

        if (it % ctx->record_interval == 0 && !ctx->delta_psi_mode) {
            RECORD_ITERATION(ctx, prev_itr);
        }

        if (is_at_interest) {
            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        iter_ctx->prev_itr = next_itr;
    }

    RECORD_ITERATION(ctx, prev_itr);
    end_iteration(iter_ctx);
}

static bool simulate_orbit_step(struct sim_ctx *ctx, bool *is_maximum,
                                scalar *prev_phi, scalar *prev_r) {
    struct sim_itr *prev_itr = ctx->iter_ctx->prev_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;

    quantum_angular angular = ctx->iter_ctx->electron_orbit->angular;

    const bool is_at_interest =
        iterate(ctx->iter_ctx, ctx->time_interval, POLAR);

    next_itr->r_dot_dot = compute_r_dot_dot(R(prev_itr), angular);
    next_itr->phi_dot = POLAR_PHI_DOT(angular, R(next_itr));

    if (!is_at_interest)
        return false;

    *is_maximum = !(*is_maximum);
    if (!(*is_maximum))
        return true;

    if (*prev_phi != 0) {
        prev_itr->delta_phi += prev_itr->phi - *prev_phi;

        if (ctx->delta_psi_mode) {
            RECORD_ITERATION(ctx, prev_itr);
        }

        next_itr->delta_phi = prev_itr->delta_phi;
    }

    *prev_r = R(prev_itr);
    *prev_phi = PHI(prev_itr);

    return true;
}