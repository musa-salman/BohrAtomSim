#ifdef __cplusplus
extern "C" {
#endif

#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>

#include "orbital_math.h"
#include "types.h"
#include "utils/macros.h"

enum sim_type : unsigned char {
    POLAR,
    REL_POLAR,
    SPHERICAL,
    REL_SPHERICAL,
    SPIN
};

struct iter_ctx {
    struct sim_itr *prev_itr;
    struct sim_itr *next_itr;

    scalar time;
};

// simulation iteration to hold the current and next iteration values
struct sim_itr {
    scalar r;      // iteration distance
    scalar r_dot;  // iteration movement speed
    scalar r_ddot; // iteration movement acceleration

    scalar phi;     // iteration angle
    scalar phi_dot; // iteration angular speed

    scalar theta;
    scalar theta_dot;
    scalar theta_ddot;

    scalar delta_phi; // iteration angle of the perihelion

    scalar gamma; // iteration rel mass mult
};

struct motion_step_2d {
    scalar time;

    scalar r;
    scalar r_dot;
    scalar r_ddot;

    scalar psi;
    scalar psi_dot;

    scalar gamma;
};

void init_motion_step(struct motion_step_2d *step, scalar r_0, scalar v_0,
                      scalar theta_rv);

static inline bool iterate(struct iter_ctx *ctx, scalar time_interval,
                           enum sim_type type) {
    const struct sim_itr *curr_itr = ctx->prev_itr;
    struct sim_itr *next_itr = ctx->next_itr;

    ctx->time += time_interval;

    next_itr->r_dot = R_DOT(curr_itr) + R_DOT_DOT(curr_itr) * time_interval;
    next_itr->r = RHO(curr_itr) + R_DOT(curr_itr) * time_interval;

    next_itr->phi = PHI(curr_itr) + PHI_DOT(curr_itr) * time_interval;

    if (PHI(next_itr) > TWO_PI) {
        next_itr->phi -= TWO_PI;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        next_itr->theta_dot =
            THETA_DOT(curr_itr) + THETA_DOT_DOT(curr_itr) * time_interval;
        next_itr->theta = THETA(curr_itr) + THETA_DOT(curr_itr) * time_interval;
    }

    return R_DOT(next_itr) * R_DOT(curr_itr) < 0;
}
void init_iteration(struct sim_itr *itr, enum sim_type type);

#endif // ITERATOR_H

#ifdef __cplusplus
}
#endif