#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include <polar/polar_calc.h>
#include <polar/polar_sim.h>
#include <utils/utils.h>

void simulate_orbit(struct sim_ctx *ctx);
bool simulate_orbit_step(struct sim_ctx *ctx, long double curr_l,
                         bool *is_maximum, long double *prev_max_vec,
                         long double *prevR, FILE *res_f);

void polar_sim_ele(struct config *config) {
    const size_t list_size = linked_list_size(config->filter_list);

    struct sim_ctx ctx = {.config = config};
    for (size_t i = 0; i < list_size; i++) {
        ctx.electron_orbit = linked_list_pop(config->filter_list);
        simulate_orbit(&ctx);
        free(ctx.electron_orbit);
    }
}

bool simulate_orbit_step(struct sim_ctx *ctx, long double curr_l,
                         bool *is_maximum, long double *prev_max_vec,
                         long double *prev_r, FILE *res_f) {
    struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;
    const struct config *config = ctx->config;
    long double K = ctx->electron_orbit->angular;

    const bool is_at_interest = iterate(ctx);

    next_itr->r_dot_dot = compute_r_dot_dot(config->electron_mass, curr_itr->r,
                                            config->electron_charge, K);
    next_itr->phi_dot =
        compute_phi_dot(curr_l, config->electron_mass, curr_itr->r);

    if (!is_at_interest)
        return false;

    *is_maximum = !(*is_maximum);

    if (*is_maximum) {
        if (*prev_max_vec != 0) {
            curr_itr->delta_phi += curr_itr->phi - *prev_max_vec;

            if (config->delta_psi_mode) {
                log_iteration(res_f, curr_itr);
            }

            next_itr->delta_phi = curr_itr->delta_phi;
        }

        *prev_r = R(curr_itr);
        printf("r = %LE PHI = %LE\n", *prev_r, *prev_max_vec);
        *prev_max_vec = PHI(curr_itr);
    }

    return is_at_interest;
}

void simulate_orbit(struct sim_ctx *ctx) {
    start_iteration(ctx);
    struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;
    struct config *config = ctx->config;

    double N = ctx->electron_orbit->principal;
    double K = ctx->electron_orbit->angular;

    FILE *res_f = linked_list_pop(config->log_files);

    struct radial_bounds *radial_bounds = compute_radial_limits(N, K);
    long double curr_l = H_BAR * K;

    long double prev_max_vec = 0;
    long double prevR = 0;

    init_iteration(curr_itr, config->type);
    init_iteration(next_itr, config->type);

    curr_itr->r = radial_bounds->r_min;
    curr_itr->r_dot_dot = compute_r_dot_dot(config->electron_mass, curr_itr->r,
                                            config->electron_charge, K);
    curr_itr->phi_dot =
        compute_phi_dot(curr_l, config->electron_mass, curr_itr->r);

    long double revolutions = config->revolutions;

    bool is_maximum = true;

    unsigned long j = 1;
    for (unsigned long it = 1; j < config->iters; it++) {
        const bool is_at_interest = simulate_orbit_step(
            ctx, curr_l, &is_maximum, &prev_max_vec, &prevR, res_f);

        if (it % config->log_p == 0 && !config->delta_psi_mode) {
            log_iteration(res_f, curr_itr);
        }

        if (config->itr_mode) {
            j++;
        } else if (is_at_interest) {
            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        const struct sim_itr *temp = curr_itr;
        ctx->curr_itr = *next_itr;
        ctx->next_itr = *temp;
    }

    log_iteration(res_f, curr_itr);
    end_iteration(ctx);
    free(radial_bounds);
    fclose(res_f);
}