#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <time.h>


#include "types.h"

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

    scalar phi;         // iteration angle
    scalar phi_dot;     // iteration angular speed
    scalar phi_dot_dot; // iteration angular acceleration

    scalar theta;
    scalar theta_dot;
    scalar theta_dot_dot;

    scalar delta_phi; // iteration angle of the perihelion

    scalar gamma; // iteration rel mass mult
    scalar epsilon;
};

void start_iteration(struct iter_ctx *ctx);

void end_iteration(struct iter_ctx *ctx);

bool iterate(struct iter_ctx *ctx, scalar time_interval, enum sim_type type);

void init_iteration(struct sim_itr *itr, enum sim_type type);

#endif // ITERATOR_H