#include <stddef.h>

#include "atom/atom_bohr_sim.h"

#include "spherical/spherical_sim_rel.h"

#include "orbital_math.h"
#include "utils/constants.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static bool simulate_orbit_step(struct sim_ctx *ctx, bool *at_max, int *sign,
                                bool *theta_flag, long double n_phi);

void simulate_spherical_rel_orbit(struct sim_ctx *ctx) {
    struct sim_itr *curr_itr = ctx->iter_ctx->prev_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;
    const struct atom *atom = ctx->atom;

    start_iteration(ctx->iter_ctx);

    const struct electron_orbit *curr_orbit = ctx->iter_ctx->electron_orbit;

    long double N = curr_orbit->principal;
    long double K = curr_orbit->angular;
    long double m = curr_orbit->magnetic;

    long double curr_l = H_BAR * K;

    struct radial_bounds radial_bounds = compute_radial_limits(N, K);

    int sign = 1;
    bool theta_flag = false;

    long double n_phi = K - m;

    long double theta_min = compute_theta_min(n_phi, K);

    bool at_max = true;

    init_iteration(curr_itr, REL_SPHERICAL);
    init_iteration(next_itr, REL_SPHERICAL);

    curr_itr->r = radial_bounds.r_min;
    curr_itr->theta = theta_min;

    long double revolutions = ctx->revolutions;

    curr_itr->gamma = compute_rel_gamma(curr_l, MASS(atom), R(curr_itr),
                                        R_DOT(curr_itr)); // in relativity

    if (m == K) {
        next_itr->phi = PI / 2;
        curr_itr->phi = PI / 2;

        // in relativity
        curr_itr->theta_dot =
            sign * SPHERICAL_THETA_DOT_REL(K, MASS(atom), R(curr_itr),
                                           GAMMA(curr_itr));

        // in relativity
        if (THETA(curr_itr) >= PI && !theta_flag) {
            sign = -1;
        } else if (THETA(curr_itr) <= 0 && theta_flag) {
            sign = 1;
        }

        curr_itr->phi_dot = 0;
        curr_itr->theta_dot_dot = 0;
    } else {

        curr_itr->phi_dot = REL_SPHERICAL_PHI_DOT(
            n_phi, THETA(curr_itr), MASS(atom), R(curr_itr), GAMMA(curr_itr));

        curr_itr->theta_dot_dot = rel_sphere_calc_theta_dot_dot(
            (spherical_calc_rel_params){.r = R(curr_itr),
                                        .r_dot = R_DOT(curr_itr),
                                        .theta = THETA(curr_itr),
                                        .theta_dot = THETA_DOT(curr_itr),
                                        .phi_dot = PHI_DOT(curr_itr),
                                        .charge = CHARGE(atom),
                                        .mass = MASS(atom),
                                        .gamma = GAMMA(curr_itr)});
    }
    next_itr->r_dot_dot =
        compute_rel_r_dot_dot(K, MASS(atom), GAMMA(curr_itr), R(curr_itr),
                              CHARGE(atom), R_DOT(curr_itr));

    RECORD_ITERATION(ctx, curr_itr);
    for (size_t it = 0; it < ctx->max_iters; it++) {

        const bool is_at_interest =
            simulate_orbit_step(ctx, &at_max, &sign, &theta_flag, n_phi);

        next_itr->gamma =
            compute_rel_gamma(curr_l, MASS(atom), R(next_itr), R_DOT(next_itr));

        // SAME
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
        curr_itr = next_itr;
        next_itr = temp;
    }

    RECORD_ITERATION(ctx, curr_itr);
    end_iteration(ctx->iter_ctx);
}

static bool simulate_orbit_step(struct sim_ctx *ctx, bool *at_max, int *sign,
                                bool *theta_flag, long double n_phi) {
    const struct atom *atom = ctx->atom;
    struct sim_itr *curr_itr = ctx->iter_ctx->prev_itr;
    struct sim_itr *next_itr = ctx->iter_ctx->next_itr;

    bool is_at_interest = iterate(ctx->iter_ctx, ctx->time_interval, SPHERICAL);

    if (ctx->iter_ctx->electron_orbit->magnetic ==
        ctx->iter_ctx->electron_orbit->angular) {
        curr_itr->theta_dot =
            (*sign) * SPHERICAL_THETA_DOT_REL(THETA(curr_itr), MASS(atom),
                                              R(curr_itr), GAMMA(curr_itr));
        next_itr->theta_dot = THETA_DOT(curr_itr);

        if (THETA(curr_itr) >= PI && !(*theta_flag)) {
            // TODO: Why not just 2 * PI - THETA(next_itr)?
            long double overflow_angle = (THETA(next_itr) - PI);
            next_itr->theta = PI - overflow_angle;

            *theta_flag = true;
            *sign = -1;
            next_itr->phi = -PHI(next_itr);
        } else if (THETA(curr_itr) <= 0 && *theta_flag) {
            next_itr->theta = -THETA(next_itr);

            *theta_flag = false;
            *sign = 1;
            next_itr->phi = -PHI(next_itr);
        }
    } else {
        next_itr->phi_dot = REL_SPHERICAL_PHI_DOT(
            n_phi, THETA(curr_itr), MASS(atom), R(curr_itr), GAMMA(curr_itr));

        next_itr->theta_dot_dot =
            rel_sphere_calc_theta_dot_dot((spherical_calc_rel_params){
                .r = R(curr_itr),
                .r_dot = R_DOT(curr_itr),
                .theta = THETA(curr_itr),
                .theta_dot = THETA_DOT(curr_itr),
                .phi_dot = PHI_DOT(curr_itr),
                .charge = CHARGE(atom),
                .mass = MASS(atom),
                .gamma = GAMMA(curr_itr),
            });
    }

    next_itr->r_dot_dot = compute_rel_r_dot_dot(
        ctx->iter_ctx->electron_orbit->angular, MASS(atom), GAMMA(curr_itr),
        R(curr_itr), CHARGE(atom), R_DOT(curr_itr));

    if (!is_at_interest)
        return false;

    *at_max = !(*at_max);
    if (!*at_max)
        return true;

    curr_itr->delta_phi = compute_angular_distance(
        THETA(curr_itr), PHI(curr_itr), THETA(next_itr), PHI(next_itr));

    if (ctx->delta_psi_mode)
        RECORD_ITERATION(ctx, curr_itr);

    next_itr->delta_phi = DELTA_PHI(curr_itr);

    return true;
}
