#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <spin/spin.h>
#include <spin/spin_calc.h>
#include <utils/types.h>
#include <polar/polar_calc.h>
#include <spherical/spherical_calc.h>
#include <utils/iterator.h>
#include <utils/linked_list.h>
#include <utils/macros.h>

void spin_sim_ele(struct config *config)
{
    struct sim_itr *curr_itr = malloc(sizeof(*curr_itr));
    struct sim_itr *next_itr = malloc(sizeof(*next_itr));

    size_t list_size = linked_list_size(config->log_files);
    bool is_interested = false;

    struct sim_ctx ctx = {
        .config = config,
        .curr_itr = *curr_itr,
        .next_itr = *next_itr};
    for (int i = 0; i < list_size; i++)
    {
        start_iteration(&ctx);
        struct orbit *curr_orbit = (struct orbit *)linked_list_pop(config->log_files);

        long double N = curr_orbit->n;
        long double K = curr_orbit->k;
        long double m = curr_orbit->m;

        struct radial_bounds *radial_bounds = compute_radial_limits(N, K);

        long double N_phi = K - m;

        long double theta_min = sphere_calc_theta_min(N_phi, K);

        FILE *res_f = (FILE *)linked_list_pop(config->log_files);

        init_iteration(curr_itr, config->type);
        init_iteration(next_itr, config->type);

        curr_itr->dr = radial_bounds->r_min;
        curr_itr->theta = theta_min;

        curr_itr->phi_dot_0 = spin_calc_phi_dot_0(
            DR(curr_itr), MASS(config), N_phi,
            HBAR(config), THETA(curr_itr));
        curr_itr->epsilon = spin_calc_epsilon(DR(curr_itr), MASS(config), CHARGE(config), THETA(curr_itr), N_phi);

        curr_itr->phi_dot = spin_calc_phi_dot(PHI_DOT_0(curr_itr), EPSILON(curr_itr));
        curr_itr->theta_dot_dot = spin_calc_theta_dot_dot(
            DR(curr_itr), R_DOT(curr_itr), THETA(curr_itr),
            THETA(curr_itr), PHI_DOT_0(curr_itr), EPSILON(curr_itr));
        curr_itr->r_dot_dot = spin_calc_r_dot_dot(
            DR(curr_itr), THETA(curr_itr), THETA_DOT(curr_itr),
            PHI_DOT_0(curr_itr), EPSILON(curr_itr), MASS(config), CHARGE(config));

        log_iteration(res_f, curr_itr);

        long double revolutions = config->revolutions;

        unsigned long j = 1;
        for (unsigned long it = 1; j < config->iters; it++)
        {

            is_interested = iterate(&ctx);

            next_itr->phi_dot_0 = spin_calc_phi_dot_0(
                DR(curr_itr), MASS(config), N_phi,
                HBAR(config), THETA(curr_itr));
            next_itr->epsilon = spin_calc_epsilon(DR(curr_itr), MASS(config),
                                                  CHARGE(config), THETA(curr_itr), N_phi);

            next_itr->phi_dot = spin_calc_phi_dot(PHI_DOT_0(curr_itr), EPSILON(curr_itr));
            next_itr->theta_dot_dot = spin_calc_theta_dot_dot(
                DR(curr_itr), R_DOT(curr_itr), THETA(curr_itr),
                THETA(curr_itr), PHI_DOT_0(curr_itr), EPSILON(curr_itr));
            next_itr->r_dot_dot = spin_calc_r_dot_dot(
                DR(curr_itr), THETA(curr_itr), THETA_DOT(curr_itr),
                PHI_DOT_0(curr_itr), EPSILON(curr_itr), MASS(config), CHARGE(config));

            if (it % LOG_P(config) == 0)
            {
                log_iteration(res_f, curr_itr);
            }
            if (config->itr_mode)
            {
                j++;
            }
            else if (is_interested)
            {
                revolutions -= 0.5;
                if (revolutions < 0)
                {
                    break;
                }
            }
            struct sim_itr *temp = curr_itr;
            curr_itr = next_itr;
            next_itr = temp;
        }

        end_iteration(&ctx);
        log_iteration(res_f, curr_itr);

        free(radial_bounds);
        fclose(res_f);

        free(curr_orbit);
    }

    free(curr_itr);
    free(next_itr);
    printf("here\n");
}