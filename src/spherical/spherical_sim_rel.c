#include <complex.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"

#include "spherical/spherical_sim_rel.h"

#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag, scalar n_phi,
                                struct electron_orbit *orbit,
                                scalar time_interval);

void simulate_spherical_rel_orbit(struct sim_ctx *ctx,
                                  struct electron_orbit orbit) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};

    INFO("simulate_spherical_rel_orbit");
    INFO("Revolutions: %LE", (scalar)ctx->revolutions);
    INFO("Time interval: %LE", ctx->time_interval);
    INFO("Orbit: %d %d %d %d", orbit.principal, orbit.angular, orbit.magnetic,
         orbit.spin);
    struct iter_ctx iter_ctx = {
        .prev_itr = &prev_itr,
        .next_itr = &next_itr,
    };

    start_iteration(&iter_ctx);

    void *record_in =
        ORBIT_RECORD_LOCATION(ctx->record_handler, orbit_hash(orbit));

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    scalar sign = 1;
    bool theta_flag = false;

    scalar n_phi = orbit.angular - orbit.magnetic;

    init_iteration(&prev_itr, REL_SPHERICAL);
    init_iteration(&next_itr, REL_SPHERICAL);

    prev_itr.r = radial_bounds.r_min;
    prev_itr.theta = compute_theta_min(n_phi, orbit.angular);

    scalar revolutions = ctx->revolutions;

    prev_itr.gamma = compute_gamma(orbit.angular, prev_itr.r, prev_itr.r_dot);

    struct vector3 *prev_max_vec = NULL;

    if (orbit.magnetic == orbit.angular) {
        next_itr.phi = HALF_PI;
        prev_itr.phi = HALF_PI;
        prev_itr.theta_dot =
            sign *
            SPHERICAL_THETA_DOT_REL(orbit.angular, prev_itr.r, prev_itr.gamma);

        if (prev_itr.theta >= PI && !theta_flag)
            sign = -1;
        else if (prev_itr.theta <= 0 && theta_flag)
            sign = 1;

        prev_itr.phi_dot = 0;
        prev_itr.theta_dot_dot = 0;
    } else {
        prev_itr.phi_dot =
            POLAR_PHI_DOT_REL(orbit.angular, prev_itr.r, prev_itr.gamma);

        prev_itr.theta_dot_dot = compute_sphere_rel_theta_dot_dot(
            prev_itr.r, prev_itr.r_dot, prev_itr.theta, prev_itr.theta_dot,
            prev_itr.phi_dot, prev_itr.gamma);
    }

    next_itr.r_dot_dot = compute_rel_r_dot_dot(orbit.angular, prev_itr.gamma,
                                               prev_itr.r, prev_itr.r_dot);

    if (!ctx->delta_psi_mode)
        RECORD_ITERATION(ctx, record_in, iter_ctx.prev_itr);
    size_t it = 0;

    scalar time_interval = ctx->time_interval;

    while (revolutions > 0) {
        const bool is_at_interest = simulate_orbit_step(
            &iter_ctx, &sign, &theta_flag, n_phi, &orbit, time_interval);

        iter_ctx.next_itr->gamma = compute_gamma(
            orbit.angular, R(iter_ctx.prev_itr), R_DOT(iter_ctx.prev_itr));

        if (it % ctx->record_interval == 0 && !ctx->delta_psi_mode)
            RECORD_ITERATION(ctx, record_in, iter_ctx.next_itr);
        if (is_at_interest) {
            // is_at_maximum = !is_at_maximum;
            if (fabsl(iter_ctx.prev_itr->r - radial_bounds.r_max) < 1e-1) {
                struct vector3 *curr_max_vec = spherical_to_cartesian(
                    R(iter_ctx.next_itr), THETA(iter_ctx.next_itr),
                    PHI(iter_ctx.next_itr));

                if (prev_max_vec != NULL) {

                    iter_ctx.prev_itr->delta_phi +=
                        compute_angular_distance(curr_max_vec, prev_max_vec);

                    RECORD_ITERATION(ctx, record_in, iter_ctx.prev_itr);

                    INFO("prev_max_vec: (%LE %LE %LE), curr_max_vec: (%LE %LE "
                         "%LE), delta_phi: %LE",
                         prev_max_vec->x, prev_max_vec->y, prev_max_vec->z,
                         curr_max_vec->x, curr_max_vec->y, curr_max_vec->z,
                         DELTA_PHI(iter_ctx.prev_itr));

                    iter_ctx.next_itr->delta_phi = DELTA_PHI(iter_ctx.prev_itr);

                    free(prev_max_vec);
                }

                prev_max_vec = curr_max_vec;
            }

            revolutions -= 0.5f;
            if (revolutions <= 0) {
                break;
            }
        }

        const scalar max_radial_error =
            fabsl(radial_bounds.r_max - iter_ctx.next_itr->r);
        const scalar min_radial_error =
            fabsl(radial_bounds.r_min - iter_ctx.next_itr->r);

        if (max_radial_error < 2 || min_radial_error < 2) {
            ctx->time_interval = 1e-15;
        } else {
            ctx->time_interval = time_interval;
        }

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }
    INFO("iter: %zu\n", it);

    if (prev_max_vec != NULL) {
        free(prev_max_vec);
    }

    end_iteration(&iter_ctx);
}

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag, scalar n_phi,
                                struct electron_orbit *orbit,
                                scalar time_interval) {
    struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    bool is_at_interest = iterate(iter_ctx, time_interval, REL_SPHERICAL);

    if (orbit->magnetic == orbit->angular) {
        prev_itr->theta_dot =
            (*sign) * SPHERICAL_THETA_DOT_REL(orbit->angular, R(prev_itr),
                                              GAMMA(prev_itr));
        next_itr->theta_dot = THETA_DOT(prev_itr);

        if (THETA(prev_itr) >= PI && !(*theta_flag)) {
            scalar overflow_angle = (THETA(next_itr) - PI);
            next_itr->theta = PI - overflow_angle;

            *theta_flag = true;
            *sign = -1;
            next_itr->phi = -PHI(next_itr);
        } else if (THETA(prev_itr) <= 0 && *theta_flag) {
            next_itr->theta = -THETA(next_itr);

            *theta_flag = false;
            *sign = 1;
            next_itr->phi = -PHI(next_itr);
        }
    } else {
        next_itr->phi_dot = compute_sphere_rel_phi_dot(
            n_phi, THETA(prev_itr), R(prev_itr), GAMMA(prev_itr));

        next_itr->theta_dot_dot = compute_sphere_rel_theta_dot_dot(
            R(prev_itr), R_DOT(prev_itr), THETA(prev_itr), THETA_DOT(prev_itr),
            PHI_DOT(prev_itr), GAMMA(prev_itr));
    }

    next_itr->r_dot_dot = compute_rel_r_dot_dot(orbit->angular, GAMMA(prev_itr),
                                                R(prev_itr), R_DOT(prev_itr));

    return is_at_interest;
}
