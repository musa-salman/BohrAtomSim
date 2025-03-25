#include <string.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "atom/result_recorders.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"
#include "utils/utils.h"

#include "spherical/spherical_sim.h"

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag, quantum_angular angular,
                                quantum_magnetic magnetic,
                                scalar time_interval);

void simulate_spherical_orbit(const struct sim_ctx ctx) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};

    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};
    struct electron_orbit orbit = ctx.orbit;

    char file_name[FILE_PATH_SIZE];
    format_output_filename(ctx.id, file_name);

    FILE *file_bin = fopen(file_name, "wb");
    if (file_bin == NULL) {
        return;
    }

    {
        const uint8_t field_names_3DNR[][MAX_FIELD_NAME] = {
            "t",       "r",     "r_dot",     "r_ddot",    "phi",
            "phi_dot", "theta", "theta_dot", "theta_ddot"};

        init_file_header(file_bin, field_names_3DNR, 9);
    }
    start_iteration(&iter_ctx);

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    scalar sign = 1;
    bool theta_flag = false;

    scalar theta_min = compute_theta_min(orbit.magnetic, orbit.angular);

    init_iteration(&prev_itr, SPHERICAL);
    init_iteration(&next_itr, SPHERICAL);

    prev_itr.r = radial_bounds.r_min;
    prev_itr.theta = theta_min;

    scalar revolutions = ctx.revolutions;

    if (orbit.magnetic == 0) {
        next_itr.phi = HALF_PI;
        prev_itr.phi = HALF_PI;
        prev_itr.theta_dot = SPHERICAL_THETA_DOT(orbit.angular, prev_itr.r);
        next_itr.theta_dot = prev_itr.theta_dot;

        prev_itr.phi_dot = 0;
        prev_itr.theta_dot_dot = 0;
    } else {
        prev_itr.phi_dot =
            compute_phi_dot_0(orbit.angular, orbit.magnetic, prev_itr.r);

        prev_itr.theta_dot_dot = compute_sphere_theta_dot_dot(
            prev_itr.r, prev_itr.r_dot, prev_itr.theta, prev_itr.theta_dot,
            prev_itr.phi_dot);
    }
    next_itr.r_dot_dot = compute_r_dot_dot(prev_itr.r, orbit.angular);

    size_t it = 0;
    while (revolutions > 0) {
        const bool is_max =
            simulate_orbit_step(&iter_ctx, &sign, &theta_flag, orbit.angular,
                                orbit.magnetic, ctx.time_interval);

        if (it % ctx.record_interval == 0) {
            record2bin(file_bin, &prev_itr);
        }

        if (is_max) {
            revolutions -= FLOAT_LITERAL_SUFFIX(0.5);
            if (revolutions <= 0) {
                break;
            }
        }

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }

    fclose(file_bin);
    end_iteration(&iter_ctx);
}

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag, quantum_angular angular,
                                quantum_magnetic magnetic,
                                scalar time_interval) {
    struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    bool is_at_interest = iterate(iter_ctx, time_interval, SPHERICAL);

    if (magnetic == 0) {
        prev_itr->theta_dot =
            (*sign) * SPHERICAL_THETA_DOT(angular, RHO(prev_itr));
        next_itr->theta_dot = THETA_DOT(prev_itr);

        if (THETA(prev_itr) >= SIM_PI && !(*theta_flag)) {
            *theta_flag = true;
            *sign = -1;
            prev_itr->phi = -PHI(prev_itr);
            next_itr->phi = -PHI(next_itr);
        } else if (THETA(prev_itr) <= 0 && *theta_flag) {
            prev_itr->theta = -THETA(prev_itr);
            *theta_flag = false;
            *sign = 1;
            prev_itr->phi = -PHI(prev_itr);
            next_itr->phi = -PHI(next_itr);
        }
    } else {
        next_itr->phi_dot =
            compute_spherical_phi_dot(magnetic, THETA(prev_itr), RHO(prev_itr));
        next_itr->theta_dot_dot = compute_sphere_theta_dot_dot(
            RHO(prev_itr), R_DOT(prev_itr), THETA(prev_itr),
            THETA_DOT(prev_itr), PHI_DOT(prev_itr));
    }

    next_itr->r_dot_dot = compute_r_dot_dot(RHO(prev_itr), angular);

    return is_at_interest;
}