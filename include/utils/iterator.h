#ifndef ITERATOR_H
#define ITERATOR_H

#include <time.h>
#include <stdbool.h>

#include "types.h"


enum sim_type: unsigned char
{
    POLAR,
    REL_POLAR,
    SPHERICAL,
    REL_SPHERICAL,
    SPIN,
    REL_SPIN
};

struct iter_ctx {
    clock_t start_time;
    clock_t end_time;

    unsigned char orbit_i;
    struct electron_orbit *electron_orbit;

    struct sim_itr *prev_itr;
    struct sim_itr *next_itr;
};

// simulation iteration to hold the current and next iteration values
struct sim_itr
{
    scalar r;         // iteration distance
    scalar r_dot;     // iteration movement speed
    scalar r_dot_dot; // iteration movement acceleration

    scalar phi;       // iteration angle
    scalar delta_phi; // iteration angle of the perihelion
    scalar initial_phi_dot;
    scalar phi_dot; // iteration angular speed

    scalar theta;
    scalar theta_dot;
    scalar theta_dot_dot;

    scalar dt;    // iteration time
    scalar gamma; // iteration rel mass mult
    scalar epsilon;
};


void start_iteration(struct iter_ctx *ctx);

void end_iteration(struct iter_ctx *ctx);

bool iterate(struct iter_ctx *ctx, scalar time_interval,
             enum sim_type type);

void init_iteration(struct sim_itr *itr, enum sim_type type);

#endif // ITERATOR_H