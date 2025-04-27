#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atom/atom_bohr_sim.h"

#include "spherical/spherical_sim_rel.h"

#include "atom/result_recorders.h"
#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"
#include "utils/utils.h"

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag,
                                const struct electron_orbit *orbit,
                                scalar time_interval);

void simulate_spherical_rel_orbit(const struct sim_ctx ctx) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};

    struct iter_ctx iter_ctx = {
        .prev_itr = &prev_itr,
        .next_itr = &next_itr,
    };

    struct electron_orbit orbit = ctx.orbit;

    char file_name[FILE_PATH_SIZE];
    format_output_filename(ctx.id, file_name);

    FILE *file_bin = fopen(file_name, "wb");
    if (file_bin == NULL) {
        return;
    }

    {
        const uint8_t field_names_3DR[][MAX_FIELD_NAME] = {
            "t",          "r",       "r_dot",     "r_ddot",
            "phi",        "phi_dot", "theta",     "theta_dot",
            "theta_ddot", "gamma",   "delta_psi",
        };

        init_file_header(file_bin, field_names_3DR, 11);
    }
    start_iteration(&iter_ctx);

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    scalar sign = 1;
    bool theta_flag = false;

    init_iteration(&prev_itr, REL_SPHERICAL);
    init_iteration(&next_itr, REL_SPHERICAL);

    prev_itr.r = radial_bounds.r_min;
    prev_itr.theta = compute_theta_min(orbit.magnetic, orbit.angular);

    scalar revolutions = ctx.revolutions;

    prev_itr.gamma = compute_gamma(orbit.angular, prev_itr.r, prev_itr.r_dot);

    struct vector3 *prev_max_vec = NULL;

    if (orbit.magnetic == 0) {
        next_itr.phi = HALF_PI;
        prev_itr.phi = HALF_PI;
        prev_itr.theta_dot =
            SPHERICAL_THETA_DOT_REL(orbit.angular, prev_itr.r, prev_itr.gamma);

        prev_itr.phi_dot = 0;
        prev_itr.theta_dot_dot = 0;
    } else {
        prev_itr.phi_dot = compute_sphere_rel_phi_dot_0(
            orbit.angular, orbit.magnetic, prev_itr.r, prev_itr.gamma);

        prev_itr.theta_dot_dot = compute_sphere_rel_theta_dot_dot(
            prev_itr.r, prev_itr.r_dot, prev_itr.theta, prev_itr.theta_dot,
            prev_itr.phi_dot, prev_itr.gamma);
    }

    next_itr.r_dot_dot = compute_rel_r_ddot(orbit.angular, prev_itr.gamma,
                                            prev_itr.r, prev_itr.r_dot);
    size_t it = 0;

    scalar time_interval = ctx.time_interval;

    while (revolutions > 0) {
        const bool is_at_interest = simulate_orbit_step(
            &iter_ctx, &sign, &theta_flag, &orbit, time_interval);

        iter_ctx.next_itr->gamma = compute_gamma(
            orbit.angular, RHO(iter_ctx.prev_itr), R_DOT(iter_ctx.prev_itr));

        if (it % ctx.record_interval == 0)
            record2bin(file_bin, iter_ctx.next_itr, iter_ctx.time);

        if (is_at_interest) {
            if (fabsl(iter_ctx.prev_itr->r - radial_bounds.r_max) < 1e-1) {
                struct vector3 *curr_max_vec = spherical_to_cartesian(
                    RHO(iter_ctx.next_itr), THETA(iter_ctx.next_itr),
                    PHI(iter_ctx.next_itr));

                if (prev_max_vec != NULL) {

                    iter_ctx.prev_itr->delta_phi +=
                        compute_angular_distance(curr_max_vec, prev_max_vec);

                    record2bin(file_bin, iter_ctx.prev_itr, iter_ctx.time);

                    INFO("prev_max_vec: (%E %E %E), curr_max_vec: (%E %E "
                         "%E), delta_phi: %E",
                         C2D(prev_max_vec->x), C2D(prev_max_vec->y),
                         C2D(prev_max_vec->z), C2D(curr_max_vec->x),
                         C2D(curr_max_vec->y), C2D(curr_max_vec->z),
                         C2D(DELTA_PHI(iter_ctx.prev_itr)));

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

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }
    INFO("iter: %zu\n", it);

    if (prev_max_vec != NULL) {
        free(prev_max_vec);
    }

    fclose(file_bin);
    end_iteration(&iter_ctx);
}

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag,
                                const struct electron_orbit *orbit,
                                scalar time_interval) {
    struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    bool is_at_interest = iterate(iter_ctx, time_interval, REL_SPHERICAL);

    if (orbit->magnetic == 0) {
        prev_itr->theta_dot =
            (*sign) * SPHERICAL_THETA_DOT_REL(orbit->angular, RHO(prev_itr),
                                              GAMMA(prev_itr));
        next_itr->theta_dot = THETA_DOT(prev_itr);

        if (THETA(prev_itr) >= SIM_PI && !(*theta_flag)) {
            *theta_flag = true;
            *sign = -1;
            next_itr->phi = -PHI(next_itr);
            prev_itr->phi = -PHI(prev_itr);
        } else if (THETA(prev_itr) <= 0 && *theta_flag) {
            *theta_flag = false;
            *sign = 1;
            next_itr->phi = -PHI(next_itr);
            prev_itr->phi = -PHI(prev_itr);
        }
    } else {
        next_itr->phi_dot = compute_sphere_rel_phi_dot(
            orbit->magnetic, THETA(prev_itr), RHO(prev_itr), GAMMA(prev_itr));

        next_itr->theta_dot_dot = compute_sphere_rel_theta_dot_dot(
            RHO(prev_itr), R_DOT(prev_itr), THETA(prev_itr),
            THETA_DOT(prev_itr), PHI_DOT(prev_itr), GAMMA(prev_itr));
    }

    next_itr->r_dot_dot = compute_rel_r_ddot(orbit->angular, GAMMA(prev_itr),
                                             RHO(prev_itr), R_DOT(prev_itr));

    return is_at_interest;
}
