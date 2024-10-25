#ifndef ATOM_BOHR_SIM_H
#define ATOM_BOHR_SIM_H

#include <Python.h>

#include "utils/iterator.h"

typedef void (*record_func)(void *record_in, const unsigned char orbit_i,
                            const unsigned char iter_i, const struct sim_itr *iter);

struct record_handler {
    void *record_in;
    record_func record;
    unsigned char curr_records;
};

#define RECORD_ITERATION(sim_ctx, curr_itr) \
    do { \
        (sim_ctx)->record_handler->record( \
            (sim_ctx)->record_handler->record_in, \
            (sim_ctx)->iter_ctx->orbit_i, \
            (sim_ctx)->record_handler->curr_records, \
            (curr_itr)); \
        (sim_ctx)->record_handler->curr_records++; \
    } while (0)

struct electron_orbit
{
    /**
     * The principal quantum number is orbital size and energy level (n)
     * n = 1, 2, 3,...
     */
    unsigned char principal;

    /**
     * The azimuthal quantum number is the orbital shape aka subshell (l)
     * l = 0, 1, 2,..., n-1
     */
    unsigned char angular;

    /**
     * The magnetic quantum number is the orbital orientation (m)
     * m = -l, -l+1, ..., 0, ..., l-1, l
     */
    signed char magnetic;

    /**
     * The spin quantum number is the electron spin (s)
     * s = -1/2, 1/2
     */
    float spin;
};

struct atom {
    long double electron_charge;
    long double electron_mass;

    struct electron_orbit *electrons;
    unsigned char electrons_count;
};

struct sim_ctx {
    struct atom *atom;

    struct record_handler *record_handler;

    struct iter_ctx *iter_ctx;

    // Simulation control
    double revolutions;
    size_t max_iters;
    long double time_interval;
    bool delta_psi_mode;
    unsigned char record_interval;
};

#endif // ATOM_BOHR_SIM_H