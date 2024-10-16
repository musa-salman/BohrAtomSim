#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdio.h>
#include <time.h>
#include "types.h"

struct sim_ctx {
    clock_t start_time;
    clock_t end_time;
    struct orbit orbit;

    struct sim_itr curr_itr;
    struct sim_itr next_itr;

    struct config *config;
};

void start_iteration(struct sim_ctx *ctx);

void end_iteration(struct sim_ctx *ctx);

bool iterate(struct sim_ctx *ctx);

void init_iteration(struct sim_itr *itr, enum sim_type type);

void log_iteration(FILE *result_f, const struct sim_itr *itr);

#endif // ITERATOR_H