#include <math.h>
#include <stddef.h>

#include "orbital_math.h"
#include "spherical/optimized_3d_rel_sim.h"
#include "utils/types.h"

#define PRINCIPAL 3
#define ANGULAR 2
#define MAGNETIC 1

#define ANGULAR_SQUARE (ANGULAR * ANGULAR)

#define DELTA_TIME 1e-7

void optimized_3d_rel_sim() {
    /*
        Initialize the simulation:
    */
    scalar r = compute_radial_limits(PRINCIPAL, ANGULAR).r_min;
    scalar theta = asin(0.5);
    scalar phi = 0;

    scalar r_dot = 0;

    /*
        compute_gamma it returns the gamma value
        for now in inv_gamma stored gamma value
        so it can be used for the other functions that require gamma
    */
    scalar inv_gamma = compute_gamma(ANGULAR, r, r_dot);
    scalar r_dot_dot = compute_rel_r_dot_dot(ANGULAR, inv_gamma, r, r_dot);

    scalar phi_dot = compute_sphere_rel_phi_dot(MAGNETIC, theta, r, inv_gamma);

    scalar theta_dot = 0;
    scalar theta_dot_dot = compute_sphere_rel_theta_dot_dot(
        r, r_dot, theta, theta_dot, phi_dot, inv_gamma);

    /*
        Precompute reused inverse gamma
    */
    inv_gamma = 1 / inv_gamma;

    scalar prev_r_dot = r_dot;

    /*
        Run the simulation:
    */
    while (r_dot * prev_r_dot >= 0) {
        /*
            Update the values:
        */
        prev_r_dot = r_dot;

        r_dot = r_dot + r_dot_dot * DELTA_TIME;
        r = r + r_dot * DELTA_TIME;

        phi = phi + phi_dot * DELTA_TIME;

        theta_dot = theta_dot + theta_dot_dot * DELTA_TIME;
        theta = theta + theta_dot * DELTA_TIME;

        /*
            Compute the new values:
        */
        const scalar inv_r = 1 / r;
        const scalar inv_r_squared = inv_r * inv_r;

        /*
            compute gamma
        */
        const scalar term1_gamma =
            inv_r_squared * SQUARE(ANGULAR) * INV_SPEED_OF_LIGHT_SQUARED;
        const scalar term2_gamma = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

        inv_gamma = sqrt((1 - term2_gamma) / (1 + term1_gamma));

        /*
            compute phi_dot
        */
        const scalar sin_theta = sin(theta);
        phi_dot = MAGNETIC / (sin_theta * sin_theta);

        /*
            compute r_dot_dot
        */
        const scalar term1_r_ddot = inv_gamma * inv_r * SQUARE(ANGULAR);
        const scalar term2_r_ddot = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

        // Precompute reused terms
        const scalar reciprocal = inv_gamma * inv_r;

        // Perform computation
        const scalar intermediate = term1_r_ddot + term2_r_ddot - 1.0;
        r_dot_dot = intermediate * reciprocal;

        /*
            compute theta_dot_dot
        */
        const scalar term1_theta_ddot = 0.5 * sin(2 * theta) * SQUARE(phi_dot);
        const scalar term2_theta_ddot = 2 * r_dot * theta_dot * inv_r;
        const scalar term3_theta_ddot =
            inv_gamma * inv_r * INV_SPEED_OF_LIGHT_SQUARED;

        theta_dot_dot =
            term1_theta_ddot - term2_theta_ddot * (1 - term3_theta_ddot);
    }
}
