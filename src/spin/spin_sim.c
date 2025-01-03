#include <stddef.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "spin/spin_sim.h"

#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct iter_ctx *iter_ctx,
                                quantum_magnetic magnetic,
                                scalar time_interval);

void simulate_spin_orbit(struct sim_ctx *ctx, struct electron_orbit orbit) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};

    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};

    start_iteration(&iter_ctx);

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    scalar theta_min = compute_theta_min(orbit.magnetic, orbit.angular);

    init_iteration(&prev_itr, SPIN);
    init_iteration(&next_itr, SPIN);

    void *record_in =
        ORBIT_RECORD_LOCATION(ctx->record_handler, orbit_hash(orbit));

    prev_itr.r = radial_bounds.r_min;
    prev_itr.theta = theta_min;

    scalar revolutions = ctx->revolutions;

    prev_itr.phi_dot =
        compute_spin_phi_dot_0(orbit.angular, orbit.magnetic, prev_itr.r);

    prev_itr.theta_dot_dot =
        compute_spin_theta_ddot(prev_itr.r, prev_itr.theta, prev_itr.phi_dot);

    next_itr.r_dot_dot = compute_spin_r_ddot(
        prev_itr.r, prev_itr.theta, prev_itr.theta_dot, prev_itr.phi_dot);

    scalar time = 0;
    size_t it = 0;
    while (revolutions > 0) {
        const bool is_max =
            simulate_orbit_step(&iter_ctx, orbit.magnetic, ctx->time_interval);

        time += ctx->time_interval;
        if (it % ctx->record_interval == 0 && !(ctx->delta_psi_mode)) {
            iter_ctx.next_itr->dt = time;
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

static bool simulate_orbit_step(struct iter_ctx *iter_ctx,
                                quantum_magnetic magnetic,
                                scalar time_interval) {
    struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    bool is_at_interest = iterate(iter_ctx, time_interval, SPHERICAL);

    next_itr->phi_dot =
        compute_spin_phi_dot(magnetic, THETA(prev_itr), R(prev_itr));
    next_itr->theta_dot_dot = compute_spin_theta_ddot(
        R(prev_itr), THETA(prev_itr), next_itr->phi_dot);

    next_itr->r_dot_dot = compute_spin_r_ddot(
        R(prev_itr), THETA(prev_itr), THETA_DOT(prev_itr), next_itr->phi_dot);
    return is_at_interest;
}