#include <stddef.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "spherical/spherical_sim.h"

#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct sim_ctx *ctx, bool *at_max, int *sign,
                                bool *theta_flag, long double n_phi,
                                struct vector3 **prev_max_vec);

void simulate_spherical_orbit(struct sim_ctx *ctx) {
    struct sim_itr *prev_itr = ctx->iter_ctx->prev_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;

    start_iteration(ctx->iter_ctx);

    const struct electron_orbit *curr_orbit = ctx->iter_ctx->electron_orbit;

    long double N = curr_orbit->principal;
    long double angular = curr_orbit->angular;
    long double m = curr_orbit->magnetic;

    struct vector3 *prev_max_vec = NULL;
    struct radial_bounds radial_bounds = compute_radial_limits(N, angular);

    int sign = 1;
    bool theta_flag = false;

    long double n_phi = angular - m;

    long double theta_min = compute_theta_min(n_phi, angular);

    bool at_max = true;

    init_iteration(prev_itr, SPHERICAL);
    init_iteration(next_itr, SPHERICAL);

    prev_itr->r = radial_bounds.r_min;
    prev_itr->theta = theta_min;

    long double revolutions = ctx->revolutions;

    if (m == angular) {
        next_itr->phi = PI / 2;
        prev_itr->phi = PI / 2;
        prev_itr->theta_dot = SPHERICAL_THETA_DOT(angular, R(prev_itr));
        next_itr->theta_dot = THETA_DOT(prev_itr);

        prev_itr->phi_dot = 0;
        prev_itr->theta_dot_dot = 0;
    } else {

        prev_itr->phi_dot =
            compute_spherical_phi_dot(n_phi, THETA(prev_itr), R(prev_itr));
        prev_itr->theta_dot_dot = compute_sphere_theta_dot_dot(
            R(prev_itr), R_DOT(prev_itr), THETA(prev_itr), THETA_DOT(prev_itr),
            PHI_DOT(prev_itr));
    }
    next_itr->r_dot_dot = compute_r_dot_dot(R(prev_itr), angular);

    RECORD_ITERATION(ctx, prev_itr);
    for (size_t it = 0; it < ctx->max_iters; it++) {

        const bool is_at_interest = simulate_orbit_step(
            ctx, &at_max, &sign, &theta_flag, n_phi, &prev_max_vec);

        if (it % ctx->record_interval == 0 && !(ctx->delta_psi_mode)) {
            RECORD_ITERATION(ctx, prev_itr);
        }

        if (is_at_interest) {
            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        prev_itr = next_itr;
    }

    RECORD_ITERATION(ctx, prev_itr);
    end_iteration(ctx->iter_ctx);
}

static bool simulate_orbit_step(struct sim_ctx *ctx, bool *at_max, int *sign,
                                bool *theta_flag, long double n_phi,
                                struct vector3 **prev_max_vec) {
    struct sim_itr *prev_itr = ctx->iter_ctx->prev_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;

    bool is_at_interest = iterate(ctx->iter_ctx, ctx->time_interval, SPHERICAL);

    if (ctx->iter_ctx->electron_orbit->magnetic ==
        ctx->iter_ctx->electron_orbit->angular) {
        prev_itr->theta_dot =
            (*sign) * SPHERICAL_THETA_DOT(
                          ctx->iter_ctx->electron_orbit->angular, R(prev_itr));
        next_itr->theta_dot = THETA_DOT(prev_itr);

        if (THETA(prev_itr) >= PI && !(*theta_flag)) {
            *theta_flag = true;
            *sign = -1;
            prev_itr->phi = -PHI(prev_itr);
            next_itr->phi = -PHI(next_itr);
        } else if (THETA(prev_itr) <= 0 && *theta_flag) {
            *theta_flag = false;
            *sign = 1;
            prev_itr->phi = -PHI(prev_itr);
            next_itr->phi = -PHI(next_itr);
        }
    } else {
        next_itr->phi_dot =
            compute_spherical_phi_dot(n_phi, THETA(prev_itr), R(prev_itr));
        next_itr->theta_dot_dot = compute_sphere_theta_dot_dot(
            R(prev_itr), R_DOT(prev_itr), THETA(prev_itr), THETA_DOT(prev_itr),
            PHI_DOT(prev_itr));
    }

    next_itr->r_dot_dot =
        compute_r_dot_dot(R(prev_itr), ctx->iter_ctx->electron_orbit->angular);

    if (!is_at_interest)
        return false;

    *at_max = !(*at_max);
    if (!*at_max)
        return true;

    struct vector3 *curr_max_vec =
        spherical_to_cartesian(R(next_itr), THETA(next_itr), PHI(next_itr));

    if (*prev_max_vec != NULL) {
        prev_itr->delta_phi =
            compute_angular_distance(curr_max_vec, *prev_max_vec);

        if (ctx->delta_psi_mode)
            RECORD_ITERATION(ctx, prev_itr);

        next_itr->delta_phi = DELTA_PHI(prev_itr);

        free(*prev_max_vec);
    }

    *prev_max_vec = curr_max_vec;

    return true;
}