#ifndef ITERATOR_H
#define ITERATOR_H

#include "types.h"
#include <time.h>

struct iter_ctx {
    clock_t start_time;
    clock_t end_time;

    unsigned char orbit_i;
    struct electron_orbit *electron_orbit;

    struct sim_itr *curr_itr;
    struct sim_itr *next_itr;
};

void start_iteration(struct iter_ctx *ctx);

void end_iteration(struct iter_ctx *ctx);

bool iterate(struct iter_ctx *ctx, long double time_interval,
             enum sim_type type);

void init_iteration(struct sim_itr *itr, enum sim_type type);

#endif // ITERATOR_H