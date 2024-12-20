#include <stddef.h>

#include "atom/atom_bohr_sim.h"

#include "polar/polar_sim_rel.h"

#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_rel_step(struct iter_ctx *ctx, scalar time_interval,
                                    quantum_angular angular);

void simulate_polar_orbit_rel(struct sim_ctx *ctx,
                              struct electron_orbit orbit) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};
    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};

    const struct record_handler *rh = ctx->record_handler;

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    scalar prev_max_vec = 0;

    init_iteration(&prev_itr, REL_POLAR);
    init_iteration(&next_itr, REL_POLAR);

    prev_itr.r = radial_bounds.r_min;

    prev_itr.gamma = compute_gamma(orbit.angular, prev_itr.r, prev_itr.r_dot);

    prev_itr.r_dot_dot = compute_rel_r_dot_dot(orbit.angular, prev_itr.gamma,
                                               prev_itr.r, prev_itr.r_dot);

    prev_itr.phi_dot =
        POLAR_PHI_DOT_REL(orbit.angular, prev_itr.r, prev_itr.gamma);
    scalar revolutions = ctx->revolutions;

    bool is_at_maximum = true;
    bool is_at_interest = false;

    size_t it = 0;
    while (revolutions > 0) {
        is_at_interest = simulate_orbit_rel_step(&iter_ctx, ctx->time_interval,
                                                 orbit.angular);

        if (it % rh->record_interval == 0 && !rh->delta_psi_mode)
            RECORD_ITERATION(ctx, iter_ctx.prev_itr);

        if (is_at_interest) {
            is_at_maximum = !is_at_maximum;
            if (is_at_maximum) {
                iter_ctx.prev_itr->delta_phi +=
                    PHI(iter_ctx.prev_itr) - prev_max_vec;
                RECORD_ITERATION(ctx, iter_ctx.prev_itr);

                iter_ctx.next_itr->delta_phi = DELTA_PHI(iter_ctx.prev_itr);

                prev_max_vec = PHI(iter_ctx.prev_itr);
            }

            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }

    end_iteration(&iter_ctx);
}

static bool simulate_orbit_rel_step(struct iter_ctx *ctx, scalar time_interval,
                                    quantum_angular angular) {
    const struct sim_itr *prev_itr = ctx->prev_itr;
    struct sim_itr *next_itr = ctx->next_itr;

    const bool is_at_interest = iterate(ctx, time_interval, REL_POLAR);

    next_itr->r_dot_dot = compute_rel_r_dot_dot(angular, GAMMA(prev_itr),
                                                R(prev_itr), R_DOT(prev_itr));

    next_itr->phi_dot =
        POLAR_PHI_DOT_REL(angular, R(prev_itr), GAMMA(prev_itr));

    next_itr->gamma = compute_gamma(angular, R(prev_itr), R_DOT(prev_itr));

    return is_at_interest;
}