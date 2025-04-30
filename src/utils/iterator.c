#include <stdbool.h>

#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/types.h"

void init_iteration(struct sim_itr *itr, enum sim_type type) {
    *itr = (struct sim_itr){
        .r = 0,
        .r_dot = 0,
        .r_ddot = 0,

        .phi = 0,
        .phi_dot = 0,

        // 3D
        .theta = -1,
        .theta_dot = -1,
        .theta_ddot = -1,

        // REL
        .delta_phi = -1,
        // spin
        .gamma = -1,
    };

    if (type == REL_POLAR || type == REL_SPHERICAL) {
        itr->gamma = 0;
        itr->delta_phi = 0;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        itr->theta = 0;
        itr->theta_dot = 0;
        itr->theta_ddot = 0;
    }
}

void init_motion_step(struct motion_step_2d *step, scalar r_0, scalar v_0,
                      scalar theta_rv) {
    const scalar angular_momentum = v_0 * r_0 * sim_sin(theta_rv);
    *step = (struct motion_step_2d){
        .r = r_0,
        .r_dot = 0,
        .r_ddot = 0,

        .phi = 0,
        .phi_dot = 0,

        .gamma = 0,
    };

    step->gamma = compute_gamma(angular_momentum, step->r, step->r_dot);
    step->r_ddot =
        compute_rel_r_ddot(angular_momentum, step->gamma, step->r, step->r_dot);
    step->phi_dot = POLAR_PHI_DOT_REL(angular_momentum, step->r, step->gamma);
}