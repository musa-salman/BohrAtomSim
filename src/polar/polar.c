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

void polar_sim_ele(struct config *config)
{
    struct sim_itr *curr_itr;
    struct sim_itr *next_itr;

    curr_itr = malloc(sizeof(struct sim_itr));
    next_itr = malloc(sizeof(struct sim_itr));

    bool is_at_interest = false;
    bool is_maximum = true;

    // TODO: what is this?
    long double Hbar_sqr = config->Hbar * config->Hbar;

    size_t listSize = linked_list_size(config->filter_list);
    struct sim_ctx ctx;
    for (int i = 0; i < listSize; i++)
    {
        ctx = (struct sim_ctx){
            .curr_itr = *curr_itr,
            .next_itr = *next_itr,
            .config = config,
        };
        start_iteration(&ctx);

        struct orbit *curr_orbit = (struct orbit *)linked_list_pop(config->filter_list);

        double N = curr_orbit->n;
        double K = curr_orbit->k;

        FILE *res_f = (FILE *)linked_list_pop(config->log_files);

        struct radial_bounds *radial_bounds = compute_radial_limits(N, K);
        long double curr_l = config->Hbar * N;
        long double K_sqr = K * K;

        long double prev_max_vec = 0;
        long double prevR = 0;

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = radial_bounds->r_min;
        curr_itr->r_dot_dot = calc_R_dot_dot(
            config->electron_mass, curr_itr->dr, config->electron_charge,
            K_sqr, Hbar_sqr);
        curr_itr->phi_dot = calc_phi_dot(curr_l, config->electron_mass, curr_itr->dr);

        long double revolutions = config->revolutions;

        unsigned long j = 1;
        for (unsigned long it = 1; j < config->iters; it++)
        {

            is_at_interest = iterate(&ctx);

            next_itr->r_dot_dot = calc_R_dot_dot(config->electron_mass,
                                                 curr_itr->dr, config->electron_charge,
                                                 K_sqr, Hbar_sqr);
            next_itr->phi_dot = calc_phi_dot(curr_l, config->electron_mass, curr_itr->dr);

            if (is_at_interest)
            {

                is_maximum = !is_maximum;

                if (is_maximum)
                {

                    if (prev_max_vec != 0)
                    {

                        curr_itr->delta_phi += curr_itr->phi - prev_max_vec;

                        if (config->delta_psi_mode)
                        {
                            log_iteration(res_f, curr_itr);
                        }

                        next_itr->delta_phi = curr_itr->delta_phi;
                    }

                    prevR = DR(curr_itr);
                    printf("r = %LE PHI = %LE\n", prevR, prev_max_vec);
                    prev_max_vec = PHI(curr_itr);
                }
            }
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
            curr_itr = next_itr;
            next_itr = temp;
        }

        log_iteration(res_f, curr_itr);
        end_iteration(&ctx);
        free(radial_bounds);
        fclose(res_f);

        free(curr_orbit);
    }

    free(curr_itr);
    free(next_itr);
}

void polar_sim_rel_ele(struct config *config)
{
    struct sim_itr *curr_itr;
    struct sim_itr *next_itr;

    // keep track of vars for iteration i adn a i+1
    curr_itr = malloc(sizeof(*curr_itr));
    next_itr = malloc(sizeof(*next_itr));

    // Hbar_sqr is the value of hbar squared
    long double Hbar_sqr = config->Hbar * config->Hbar;
    // reached peak of the eclipse
    bool at_max = true;
    // start calculating
    size_t list_size = linked_list_size(config->filter_list);
    bool is_at_interest = false;

    for (int i = 0; i < list_size; i++)
    {
        struct orbit *currOrbit = (struct orbit *)linked_list_pop(config->filter_list);

        double N = currOrbit->n;
        double K = currOrbit->k;

        FILE *res_f = (FILE *)linked_list_pop(config->log_files);

        // The multiplier of H_BAR
        struct radial_bounds *radial_bounds = compute_radial_limits(N, K);
        // L is the value of H_Bar
        long double curr_l = config->Hbar * K;
        long double K_sqr = K * K;

        long double prev_max_vec = 0;

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = radial_bounds->r_min;

        curr_itr->gamma = calc_rel_gamma(curr_l, config->electron_mass, curr_itr->dr, curr_itr->r_dot);

        curr_itr->r_dot_dot = calc_rel_r_dot_dot(K_sqr * Hbar_sqr, config->electron_mass, curr_itr->gamma, curr_itr->dr,
                                                 config->electron_charge, curr_itr->r_dot);

        curr_itr->phi_dot = calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->dr, config->electron_mass);

        long double revolutions = config->revolutions;

        unsigned long j = 1;
        struct sim_ctx ctx = {
            .curr_itr = *curr_itr,
            .next_itr = *next_itr,
            .config = config,
        };
        for (unsigned long it = 1; j < config->iters; it++)
        {

            is_at_interest = iterate(&ctx);

            next_itr->r_dot_dot = calc_rel_r_dot_dot(
                K_sqr * Hbar_sqr, config->electron_mass, curr_itr->gamma,
                curr_itr->dr, config->electron_charge, curr_itr->r_dot);

            next_itr->phi_dot = calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->dr, config->electron_mass);

            if (is_at_interest)
            {
                at_max = !at_max;
                if (at_max)
                {

                    long double chi = calc_rel_chi(config->electron_mass, config->electron_charge, K);

                    if (prev_max_vec != 0)
                    {

                        curr_itr->delta_phi += PHI(curr_itr) - prev_max_vec;
                        if (config->delta_psi_mode)
                        {
                            log_iteration(res_f, curr_itr);
                        }
                        next_itr->delta_phi = DELTA_PHI(curr_itr);

                        printf(" currMaxth - prevMaxVec  %LE, acurrate %LE \n", PHI(curr_itr) - prev_max_vec, (((2 * PI) / chi) - 2 * PI));
                    }

                    prev_max_vec = PHI(curr_itr);
                }
            }

            next_itr->gamma = calc_rel_gamma(curr_l, config->electron_mass, DR(curr_itr), R_DOT(curr_itr));

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
            curr_itr = next_itr;
            next_itr = temp;
        }

        free(radial_bounds);
        end_iteration(&ctx);
        log_iteration(res_f, curr_itr);

        fclose(res_f);
        free(currOrbit);
    }

    free(curr_itr);
    free(next_itr);
}