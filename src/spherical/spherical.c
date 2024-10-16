#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <spherical/spherical.h>
#include <spherical/spherical_calc.h>
#include <utils/macros.h>
#include <utils/types.h>
#include <utils/linked_list.h>
#include <utils/iterator.h>

void spherical_sim_ele(struct config *config)
{
    struct sim_itr *curr_itr = (struct sim_itr *)malloc(sizeof(struct sim_itr));
    struct sim_itr *next_itr = (struct sim_itr *)malloc(sizeof(struct sim_itr));

    long double Hbar_sqr = HBAR(config) * HBAR(config);
    int listSize = linked_list_size(config->filter_list);

    bool is_at_interest = false;
    struct sim_ctx ctx = {
        .config = config,
        .curr_itr = *curr_itr,
        .next_itr = *next_itr};

    for (int i = 0; i < listSize; i++)
    {
        start_iteration(&ctx);

        struct orbit *curr_orbit = (struct orbit *)linked_list_pop(config->filter_list);

        double N = curr_orbit->n;
        double K = curr_orbit->k;
        double m = curr_orbit->m;

        double curr_l = HBAR(config) * K;
        double K_sqr = K * K;

        double *rMinMax = calc_rmin_rmax(N, K);

        int sign = 1;
        bool theta_flag = false;

        double N_phi = K - m;

        double theta_min = sphere_calc_theta_min(N_phi, K);

        double prev_phi = 0;
        double *prev_max_vec = NULL;
        double *curr_max_vec = NULL;
        double prevR = 0;
        bool at_max = true;

        FILE *res_f = (FILE *)linked_list_pop(config->log_files);

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = rMinMax[0];
        curr_itr->theta = theta_min;

        double revolutions = config->revolutions;

        if (m == K)
        {

            next_itr->phi = PI / 2;
            curr_itr->phi = PI / 2;
            curr_itr->theta_dot = sphere_calc_spc_case_theta_dot(K, HBAR(config), MASS(config), DR(curr_itr));
            next_itr->theta_dot = THETA_DOT(curr_itr);

            curr_itr->phi_dot = 0;
            curr_itr->theta_dot_dot = 0;
        }
        else
        {

            curr_itr->phi_dot = sphere_calc_phi_dot(N_phi, HBAR(config), THETA(curr_itr),
                                                    MASS(config), DR(curr_itr));
            curr_itr->theta_dot_dot = sphere_calc_theta_dot_dot(
                DR(curr_itr), R_DOT(curr_itr),
                THETA(curr_itr), THETA_DOT(curr_itr), PHI_DOT(curr_itr));
        }
        next_itr->r_dot_dot = sphere_calc_r_dot_dot(MASS(config), DR(curr_itr), CHARGE(config), K_sqr, Hbar_sqr);

        log_iteration(res_f, curr_itr);

        struct sim_ctx ctx = {
            .config = config,
            .curr_itr = *curr_itr,
            .next_itr = *next_itr};
        unsigned long j = 1;
        for (unsigned long it = 1; j < config->iters; it++)
        {

            is_at_interest = iterate(&ctx);

            if (m == K)
            {
                curr_itr->theta_dot = sign * sphere_calc_spc_case_theta_dot(K, HBAR(config), MASS(config), DR(curr_itr));
                next_itr->theta_dot = THETA_DOT(curr_itr);

                if (THETA(curr_itr) >= PI && !(theta_flag))
                {
                    theta_flag = true;
                    sign = -1;
                    curr_itr->phi = -PHI(curr_itr);
                    next_itr->phi = -PHI(next_itr);
                }
                else if (THETA(curr_itr) <= 0 && theta_flag)
                {
                    theta_flag = false;
                    sign = 1;
                    curr_itr->phi = -PHI(curr_itr);
                    next_itr->phi = -PHI(next_itr);
                }
            }
            else
            {
                next_itr->phi_dot = sphere_calc_phi_dot(N_phi, HBAR(config), THETA(curr_itr),
                                                        MASS(config), DR(curr_itr));
                next_itr->theta_dot_dot = sphere_calc_theta_dot_dot(DR(curr_itr), R_DOT(curr_itr),
                                                                    THETA(curr_itr), THETA_DOT(curr_itr),
                                                                    PHI_DOT(curr_itr));
            }

            next_itr->r_dot_dot = sphere_calc_r_dot_dot(MASS(config), DR(curr_itr), CHARGE(config), K_sqr, Hbar_sqr);

            if (is_at_interest)
            {

                at_max = !at_max;
                if (at_max)
                {

                    curr_max_vec = stoc(DR(next_itr), PHI(next_itr), THETA(next_itr));

                    if (prev_max_vec != NULL)
                    {

                        curr_itr->delta_phi = rel_sphere_calc_deltaPhi(curr_max_vec, prev_max_vec, rMinMax[1]);

                        if (config->delta_psi_mode)
                        {

                            log_iteration(res_f, curr_itr);
                        }

                        next_itr->delta_phi = DELTA_PHI(curr_itr);

                        free(prev_max_vec);
                    }

                    prevR = DR(curr_itr);

                    prev_max_vec = curr_max_vec;
                }
            }
            if (m == 0)
            {
                prev_phi = PHI(curr_itr);
            }
            else if (m == K)
            {
                prev_phi = THETA(curr_itr);
            }

            if (it % LOG_P(config) == 0 && !(config->delta_psi_mode))
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
        endTime(*curr_orbit);
        free(rMinMax);
        fclose(res_f);
        free(curr_orbit);
    }

    free(curr_itr);
    free(next_itr);
}

void spherical_sim_rel_ele(struct config *config)
{
    struct sim_itr *curr_itr = (struct sim_itr *)malloc(sizeof(struct sim_itr)),
                   *next_itr = (struct sim_itr *)malloc(sizeof(struct sim_itr));

    bool is_interested = false;

    long double Hbar_sqr = HBAR(config) * HBAR(config);

    size_t list_size = linked_list_size(config->filter_list);

    struct sim_ctx ctx = {
        .config = config,
        .curr_itr = *curr_itr,
        .next_itr = *next_itr};

    for (int i = 0; i < list_size; i++)
    {
        start_iteration(&ctx);
        struct orbit *curr_orbit = (struct orbit *)linked_list_pop(config->filter_list);

        double N = curr_orbit->n;
        double K = curr_orbit->k;
        double m = curr_orbit->m;

        double curr_l = HBAR(config) * K;
        double K_sqr = K * K;

        double alpha = calc_alpha(CHARGE(config), HBAR(config));

        double w = calc_rel_w(N, K, MASS(config), alpha);

        double a = calc_rel_A(MASS(config), w);

        double b = calc_rel_B(MASS(config), CHARGE(config), w);

        double c = calc_rel_C(K_sqr * Hbar_sqr, CHARGE(config));

        double rel_rmin = calc_rel_rmin(a, b, c);

        double *rMinMax = calc_rmin_rmax(N, K);

        double chi = calc_rel_chi(HBAR(config), CHARGE(config), (double)(K));

        int sign = 1;
        bool theta_flag = false;

        double prevPhi = 0;
        double *prev_max_vec = NULL;
        double *curr_max_vec = NULL;
        double prevR = 0;
        bool at_max = true;

        double N_phi = K - m;

        double theta_min = rel_sphere_calc_theta_min(N_phi, K);
        double revolutions = config->revolutions;

        FILE *res_f = (FILE *)linked_list_pop(config->log_files);

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = rMinMax[0];

        curr_itr->gamma = calc_rel_gamma(curr_l, MASS(config), DR(curr_itr), R_DOT(curr_itr));

        THETA(curr_itr) = theta_min;
        //------------------------- Itr zero -------------------------
        if (m == K)
        {
            next_itr->phi = PI / 2;
            curr_itr->phi = PI / 2;

            curr_itr->theta_dot = sign * rel_sphere_calc_spc_case_theta_dot(
                                             K, HBAR(config), MASS(config),
                                             DR(curr_itr), GAMMA(curr_itr));

            if (THETA(curr_itr) >= PI && !(theta_flag))
            {
                sign = -1;
            }
            else if (THETA(curr_itr) <= 0 && theta_flag)
            {
                sign = 1;
            }

            curr_itr->phi_dot = 0;
            curr_itr->theta_dot_dot = 0;
        }
        else
        {

            curr_itr->phi_dot = rel_sphere_calc_phi_dot(
                N_phi, HBAR(config), THETA(curr_itr),
                MASS(config), DR(curr_itr), GAMMA(curr_itr));
            curr_itr->theta_dot_dot = rel_sphere_calc_theta_dot_dot(
                DR(curr_itr), R_DOT(curr_itr),
                THETA(curr_itr), THETA_DOT(curr_itr), PHI_DOT(curr_itr),
                CHARGE(config), MASS(config), GAMMA(curr_itr));
        }

        curr_itr->r_dot_dot = rel_sphere_calc_r_dot_dot(
            K_sqr, Hbar_sqr, MASS(config),
            GAMMA(curr_itr), DR(curr_itr), CHARGE(config),
            R_DOT(curr_itr));

        log_iteration(res_f, curr_itr);
        //------------------------- Itr zero -------------------------

        unsigned long j = 1;
        for (unsigned long it = 1; j < config->iters; it++)
        {

            is_interested = iterate(&ctx);

            if (m == K)
            {

                next_itr->theta_dot = sign * rel_sphere_calc_spc_case_theta_dot(
                                                 K, HBAR(config), MASS(config),
                                                 DR(curr_itr), GAMMA(curr_itr));

                if (THETA(next_itr) >= PI && !(theta_flag))
                {

                    double overValue = (THETA(next_itr) - PI);
                    next_itr->theta = PI - overValue;

                    theta_flag = true;
                    sign = -1;

                    next_itr->phi = -PHI(next_itr);
                }
                else if (THETA(next_itr) <= 0 && theta_flag)
                {

                    next_itr->theta = -THETA(next_itr);

                    theta_flag = false;
                    sign = 1;
                    next_itr->phi = -PHI(next_itr);
                }
            }
            else
            {
                next_itr->phi_dot = rel_sphere_calc_phi_dot(
                    N_phi, HBAR(config), THETA(curr_itr), MASS(config),
                    DR(curr_itr), GAMMA(curr_itr));

                next_itr->theta_dot_dot = rel_sphere_calc_theta_dot_dot(
                    DR(curr_itr), R_DOT(curr_itr), THETA(curr_itr),
                    THETA_DOT(curr_itr), PHI_DOT(curr_itr),
                    CHARGE(config), MASS(config), GAMMA(curr_itr));
            }

            next_itr->r_dot_dot = rel_sphere_calc_r_dot_dot(
                K_sqr, Hbar_sqr, MASS(config),
                GAMMA(curr_itr), DR(curr_itr), CHARGE(config),
                R_DOT(curr_itr));

            if (is_interested)
            {
                at_max = !at_max;
                if (at_max)
                {

                    curr_max_vec = stoc(DR(next_itr), PHI(next_itr), THETA(next_itr));

                    if (prev_max_vec != NULL)
                    {

                        curr_itr->delta_phi = rel_sphere_calc_deltaPhi(
                            curr_max_vec, prev_max_vec, rMinMax[1]);

                        if (config->delta_psi_mode)
                        {

                            log_iteration(res_f, curr_itr);
                        }

                        next_itr->delta_phi = DELTA_PHI(curr_itr);

                        free(prev_max_vec);
                    }

                    prevR = DR(curr_itr);

                    prev_max_vec = curr_max_vec;
                }
            }
            if (m == 0)
            {
                prevPhi = PHI(curr_itr);
            }
            else if (m == K)
            {
                prevPhi = THETA(curr_itr);
            }

            next_itr->gamma = calc_rel_gamma(curr_l, MASS(config), DR(curr_itr),
                                             R_DOT(curr_itr));

            if (it % LOG_P(config) == 0 && !(config->delta_psi_mode))
            {
                log_iteration(res_f, next_itr);
            }
            if (config->itr_mode)
            {
                j++;
            }
            else if (is_interested)
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

        log_iteration(res_f, next_itr);

        free(curr_max_vec);

        end_iteration(&ctx);
        free(rMinMax);
        fclose(res_f);
        free(curr_orbit);
    }

    free(curr_itr);
    free(next_itr);
}