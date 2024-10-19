#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <polar/polar.h>
#include <polar/polar_calc.h>
#include <polar/polar_calc_rel.h>
#include <utils/linked_list.h>
#include <utils/types.h>
#include <utils/iterator.h>
#include <utils/macros.h>

void simulate_orbit(struct sim_ctx *ctx);
bool simulate_orbit_step(struct sim_ctx *ctx, long double curr_l, long double Hbar_sqr,
                         bool *is_maximum, long double *prev_max_vec, long double *prevR, FILE *res_f);

void simulate_orbit_rel(struct sim_ctx *ctx);
bool simulate_orbit_rel_step(struct sim_ctx *ctx, long double curr_l, long double Hbar_sqr,
                             bool *is_maximum, long double *prev_max_vec, FILE *res_f);

void polar_sim_ele(struct config *config)
{
    struct sim_itr curr_itr;
    struct sim_itr next_itr;

    const size_t list_size = linked_list_size(config->filter_list);

    struct sim_ctx ctx = {
        .config = config,
        .curr_itr = curr_itr,
        .next_itr = next_itr};
    for (int i = 0; i < list_size; i++)
    {
        ctx.electron_orbit = (struct electron_orbit *)linked_list_pop(config->filter_list);
        simulate_orbit(&ctx);
        free(ctx.electron_orbit);
    }
}

bool simulate_orbit_step(struct sim_ctx *ctx, long double curr_l, long double Hbar_sqr, bool *is_maximum,
                         long double *prev_max_vec, long double *prev_r, FILE *res_f)
{
    struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;
    const struct config *config = ctx->config;
    long double K = ctx->electron_orbit->angular;
    long double K_sqr = K * K;

    const bool is_at_interest = iterate(&ctx);

    next_itr->r_dot_dot = compute_r_dot_dot(config->electron_mass, curr_itr->r,
                                            config->electron_charge, K_sqr, Hbar_sqr);
    next_itr->phi_dot = compute_phi_dot(curr_l, config->electron_mass, curr_itr->r);

    if (!is_at_interest)
        return false;

    *is_maximum = !(*is_maximum);

    if (*is_maximum)
    {
        if (*prev_max_vec != 0)
        {
            curr_itr->delta_phi += curr_itr->phi - *prev_max_vec;

            if (config->delta_psi_mode)
            {
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

void simulate_orbit(struct sim_ctx *ctx)
{
    start_iteration(&ctx);
    struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;
    struct config *config = ctx->config;

    double N = ctx->electron_orbit->principal;
    double K = ctx->electron_orbit->angular;

    FILE *res_f = linked_list_pop(config->log_files);

    struct radial_bounds *radial_bounds = compute_radial_limits(N, K);
    long double curr_l = config->Hbar * K;
    long double K_sqr = K * K;

    // TODO: what is this?
    long double Hbar_sqr = HBAR(config) * HBAR(config);

    long double prev_max_vec = 0;
    long double prevR = 0;

    init_iteration(curr_itr, config->type);
    init_iteration(next_itr, config->type);

    curr_itr->r = radial_bounds->r_min;
    curr_itr->r_dot_dot = compute_r_dot_dot(
        config->electron_mass, curr_itr->r, config->electron_charge,
        K_sqr, Hbar_sqr);
    curr_itr->phi_dot = compute_phi_dot(curr_l, config->electron_mass, curr_itr->r);

    long double revolutions = config->revolutions;

    bool is_maximum = true;

    unsigned long j = 1;
    for (unsigned long it = 1; j < config->iters; it++)
    {
        const bool is_at_interest = simulate_orbit_step(
            ctx, curr_l, Hbar_sqr, &is_maximum,
            &prev_max_vec, &prevR, res_f);

        if (it % config->log_p == 0 && !config->delta_psi_mode)
        {
            log_iteration(res_f, curr_itr);
        }

        if (config->itr_mode)
        {
            j++;
        }
        else if (is_at_interest)
        {
            revolutions -= 0.5;
            if (revolutions <= 0)
            {
                break;
            }
        }

        const struct sim_itr *temp = curr_itr;
        ctx->curr_itr = *next_itr;
        ctx->next_itr = *temp;
    }

    log_iteration(res_f, curr_itr);
    end_iteration(&ctx);
    free(radial_bounds);
    fclose(res_f);
}

void polar_sim_rel_ele(struct config *config)
{
    struct sim_itr curr_itr;
    struct sim_itr next_itr;

    size_t list_size = linked_list_size(config->filter_list);
    struct sim_ctx ctx = {
        .config = config,
        .curr_itr = curr_itr,
        .next_itr = next_itr};
    for (int i = 0; i < list_size; i++)
    {
        ctx.electron_orbit = (struct electron_orbit *)linked_list_pop(config->filter_list);
        simulate_orbit_rel(&ctx);
        free(ctx.electron_orbit);
    }
}

bool simulate_orbit_rel_step(struct sim_ctx *ctx, long double curr_l, long double Hbar_sqr,
                             bool *is_maximum, long double *prev_max_vec, FILE *res_f)
{
    const struct config *config = ctx->config;
    struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;
    long double K = ctx->electron_orbit->angular;
    long double K_sqr = K * K;
    const bool is_at_interest = iterate(&ctx);

    next_itr->r_dot_dot = calc_rel_r_dot_dot(
        K_sqr * Hbar_sqr, config->electron_mass, curr_itr->gamma,
        curr_itr->r, config->electron_charge, curr_itr->r_dot);

    next_itr->phi_dot = calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->r, config->electron_mass);

    if (!is_at_interest)
        return false;
    *is_maximum = !(*is_maximum);
    if (*is_maximum)
    {
        long double chi = calc_rel_chi(config->electron_mass, config->electron_charge, ctx->electron_orbit->angular);

        if (*prev_max_vec != 0)
        {

            curr_itr->delta_phi += PHI(curr_itr) - *prev_max_vec;
            if (config->delta_psi_mode)
            {
                log_iteration(res_f, curr_itr);
            }
            next_itr->delta_phi = DELTA_PHI(curr_itr);

            printf(" currMaxth - prevMaxVec  %LE, accurate %LE \n", PHI(curr_itr) - *prev_max_vec,
                   (((2 * PI) / chi) - 2 * PI));
        }

        *prev_max_vec = PHI(curr_itr);
    }

    next_itr->gamma = calc_rel_gamma(curr_l, config->electron_mass, R(curr_itr), R_DOT(curr_itr));
    return is_at_interest;
}

void simulate_orbit_rel(struct sim_ctx *ctx)
{
    struct config *config = ctx->config;
    struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;

    double N = ctx->electron_orbit->principal;
    double K = ctx->electron_orbit->angular;

    FILE *res_f = linked_list_pop(config->log_files);

    // The multiplier of H_BAR
    struct radial_bounds *radial_bounds = compute_radial_limits(N, K);
    // L is the value of H_Bar
    long double curr_l = config->Hbar * K;
    long double K_sqr = K * K;

    long double prev_max_vec = 0;

    long double Hbar_sqr = HBAR(config) * HBAR(config);

    init_iteration(curr_itr, config->type);
    init_iteration(next_itr, config->type);

    curr_itr->r = radial_bounds->r_min;

    curr_itr->gamma = calc_rel_gamma(curr_l, config->electron_mass, curr_itr->r, curr_itr->r_dot);

    curr_itr->r_dot_dot = calc_rel_r_dot_dot(K_sqr * Hbar_sqr, config->electron_mass, curr_itr->gamma, curr_itr->r,
                                             config->electron_charge, curr_itr->r_dot);

    curr_itr->phi_dot = calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->r, config->electron_mass);

    long double revolutions = config->revolutions;

    unsigned long j = 1;
    bool at_max = true;
    bool is_at_interest = false;
    struct sim_ctx ctx = {
        .curr_itr = *curr_itr,
        .next_itr = *next_itr,
        .config = config,
    };
    for (unsigned long it = 1; j < config->iters; it++)
    {
        is_at_interest = simulate_orbit_rel_step(
            &ctx, curr_l, Hbar_sqr, &at_max,
            &prev_max_vec, res_f);

        if (it % config->log_p == 0 && !(config->delta_psi_mode))
        {
            log_iteration(res_f, curr_itr);
        }
        if (config->itr_mode)
        {
            j++;
        }
        else if (is_at_interest)
        {
            revolutions -= 0.5;
            if (revolutions <= 0)
            {
                break;
            }
        }
        struct sim_itr *temp = curr_itr;
        ctx->curr_itr = *next_itr;
        ctx->next_itr = *temp;
    }

    free(radial_bounds);
    end_iteration(&ctx);
    log_iteration(res_f, curr_itr);

    fclose(res_f);
}