#include <stddef.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "spherical/spherical_sim.h"

#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag, scalar n_phi,
                                quantum_angular angular,
                                quantum_magnetic magnetic,
                                scalar time_interval);

void simulate_spherical_orbit(struct sim_ctx *ctx,
                              struct electron_orbit orbit) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};

    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};

    start_iteration(&iter_ctx);

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    scalar sign = 1;
    bool theta_flag = false;

    scalar n_phi = orbit.angular - orbit.magnetic;

    scalar theta_min = compute_theta_min(orbit.magnetic, orbit.angular);

    init_iteration(&prev_itr, SPHERICAL);
    init_iteration(&next_itr, SPHERICAL);

    void *record_in =
        ORBIT_RECORD_LOCATION(ctx->record_handler, orbit_hash(orbit));

    prev_itr.r = radial_bounds.r_min;
    prev_itr.theta = theta_min;

    scalar revolutions = ctx->revolutions;

    if (orbit.magnetic == orbit.angular) {
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
        const bool is_max = simulate_orbit_step(
            &iter_ctx, &sign, &theta_flag, n_phi, orbit.angular, orbit.magnetic,
            ctx->time_interval);

        if (it % ctx->record_interval == 0 && !(ctx->delta_psi_mode)) {
            RECORD_ITERATION(ctx, record_in, iter_ctx.next_itr);
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

    end_iteration(&iter_ctx);
}

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar *sign,
                                bool *theta_flag, scalar n_phi,
                                quantum_angular angular,
                                quantum_magnetic magnetic,
                                scalar time_interval) {
    struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    bool is_at_interest = iterate(iter_ctx, time_interval, SPHERICAL);

    if (magnetic == angular) {
        prev_itr->theta_dot =
            (*sign) * SPHERICAL_THETA_DOT(angular, R(prev_itr));
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

    next_itr->r_dot_dot = compute_r_dot_dot(R(prev_itr), angular);

    return is_at_interest;
}