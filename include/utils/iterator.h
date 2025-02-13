#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <time.h>

#include "orbital_math.h"
#include "types.h"
#include "utils/macros.h"

enum sim_type : unsigned char {
    POLAR,
    REL_POLAR,
    SPHERICAL,
    REL_SPHERICAL,
    SPIN,
    REL_SPIN
};

struct iter_ctx {
    struct sim_itr *prev_itr;
    struct sim_itr *next_itr;

    clock_t start_time;
    clock_t end_time;
};

// simulation iteration to hold the current and next iteration values
struct sim_itr {
    scalar dt;        // iteration time
    scalar r;         // iteration distance
    scalar r_dot;     // iteration movement speed
    scalar r_dot_dot; // iteration movement acceleration

    scalar phi;     // iteration angle
    scalar phi_dot; // iteration angular speed

    scalar theta;
    scalar theta_dot;
    scalar theta_dot_dot;

    scalar delta_phi; // iteration angle of the perihelion

    scalar gamma; // iteration rel mass mult
};

void start_iteration(struct iter_ctx *ctx);

void end_iteration(struct iter_ctx *ctx);

static inline bool iterate(struct iter_ctx *ctx, scalar time_interval,
                           enum sim_type type) {
    const struct sim_itr *curr_itr = ctx->prev_itr;
    struct sim_itr *next_itr = ctx->next_itr;

    next_itr->dt += time_interval;

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