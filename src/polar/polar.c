#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <polar/polar.h>
#include <utils/linked_list.h>
#include <utils/types.h>
#include <utils/iterator.h>
#include <utils/macros.h>

void polar_sim_ele(struct config *config)
{
    struct sim_itr *curr_itr, *next_itr;

    curr_itr = malloc(sizeof(struct sim_itr));
    next_itr = malloc(sizeof(struct sim_itr));

    bool is_at_interest = false,
    is_maximum = true;

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

        double *rMinMax = calc_rmin_rmax(N, K);
        double curr_l = config->Hbar * N;
        double K_sqr = K * K;

        double prev_phi = 0;
        double prev_max_vec = 0;
        double prevR = 0;

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = rMinMax[0];
        curr_itr->r_dot_dot = calc_R_dot_dot(config->electron_mass, curr_itr->dr, config->electron_charge, K_sqr, Hbar_sqr);
        curr_itr->phi_dot = calc_phi_dot(curr_l, config->electron_mass, curr_itr->dr);

        double revolutions = config->revolutions;

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
                    printf("r = %E PHI = %E\n", prevR, prev_max_vec);
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
        free(rMinMax);
        fclose(res_f);

        free(curr_orbit);
    }

    free(curr_itr);
    free(next_itr);
}

void polar_sim_rel_ele(struct config *config)
{
    struct sim_itr *curr_itr, *next_itr;

    // keep track of vars for iteration i adn a i+1
    curr_itr = malloc(sizeof(struct sim_itr));
    next_itr = malloc(sizeof(struct sim_itr));

    // Hbar_sqr is the value of hbar squared
    double Hbar_sqr = config->Hbar * config->Hbar;
    // reached peak of the eclipse
    bool at_max = true;
    // start calculating
    int list_size = linked_list_size(config->filter_list);
    bool is_at_interest = false;

    struct sim_ctx ctx;
    for (int i = 0; i < list_size; i++)
    {

        ctx = (struct sim_ctx){
            .curr_itr = *curr_itr,
            .next_itr = *next_itr,
            .config = config,
        };
        struct orbit *currOrbit = (struct orbit *)linked_list_pop(config->filter_list);

        double N = currOrbit->n;
        double K = currOrbit->k;

        FILE *res_f = (FILE *)linked_list_pop(config->log_files);

        // printf(" n-i = %d\n",(N-i));

        // The multiplier of H_BAR
        double *rMinMax = calc_rmin_rmax(N, K);
        // L is the value of H_Bar
        double curr_l = config->Hbar * K;
        double alpha = calc_alpha(config->electron_mass, config->electron_charge);
        double w = calc_rel_w(N, K, config->electron_mass, alpha);

        double K_sqr = K * K;
        // K^2 for Hbar_sqr

        double a = calc_rel_A(config->electron_mass, w);

        double b = calc_rel_B(config->electron_mass, config->electron_charge, w);
        double c = calc_rel_C(K_sqr * Hbar_sqr, config->electron_charge);

        double rel_rmin = calc_rel_rmin(a, b, c);
        // printf("relativistic rmin = %E ,non relativistic rmin = %E\n\n",rel_rmin,R_MIN);

        double prev_phi = 0;
        double prev_max_vec = 0;
        double prev_r = 0;

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = rMinMax[0];

        curr_itr->gamma = calc_rel_gamma(curr_l, config->electron_mass, curr_itr->dr, curr_itr->r_dot);

        curr_itr->r_dot_dot = calc_rel_r_dot_dot(K_sqr * Hbar_sqr, config->electron_mass, curr_itr->gamma, curr_itr->dr,
                                                 config->electron_charge, curr_itr->r_dot);

        curr_itr->phi_dot = calc_rel_phi_dot(curr_l, curr_itr->gamma, curr_itr->dr, config->electron_mass);

        double revolutions = config->revolutions;

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

                    double chi = calc_rel_chi(config->electron_mass, config->electron_charge, K);

                    if (prev_max_vec != 0)
                    {

                        curr_itr->delta_phi += PHI(curr_itr) - prev_max_vec;
                        if (config->delta_psi_mode)
                        {
                            log_iteration(res_f, curr_itr);
                        }
                        next_itr->delta_phi = DELTA_PHI(curr_itr);

                        printf(" currMaxth - prevMaxVec  %E, acurrate %E \n", PHI(curr_itr) - prev_max_vec, (((2 * PI) / chi) - 2 * PI));
                    }

                    prev_r = DR(curr_itr);
                    prev_max_vec = PHI(curr_itr);
                }
            }

            prev_phi = PHI(curr_itr);

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

        free(rMinMax);
        endTime(*currOrbit);
        log_iteration(res_f, curr_itr);

        fclose(res_f);
        free(currOrbit);
    }

    free(curr_itr);
    free(next_itr);
}