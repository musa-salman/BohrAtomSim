#ifndef ATOM_BOHR_SIM_H
#define ATOM_BOHR_SIM_H

#include <stdbool.h>

#include "utils/iterator.h"
#include "utils/types.h"

typedef void (*record_func)(void *record_in, const unsigned char orbit_i,
                             const struct sim_itr *iter);

struct record_handler {
    void *record_in;
    record_func record;
    unsigned char curr_records;
};

#define RECORD_ITERATION(sim_ctx, curr_itr)    (sim_ctx)->record_handler->record( \
            (sim_ctx)->record_handler->record_in, \
            (sim_ctx)->iter_ctx->orbit_i, \
            (curr_itr)) \

struct electron_orbit
{
    /**
     * The principal quantum number is orbital size and energy level (n)
     * n = 1, 2, 3,...
     */
    quantum_principle principal;

    /**
     * The azimuthal quantum number is the orbital shape aka subshell (l)
     * l = 0, 1, 2,..., n-1
     */
    quantum_angular angular;

    /**
     * The magnetic quantum number is the orbital orientation (m)
     * m = -l, -l+1, ..., 0, ..., l-1, l
     */
    quantum_magnetic magnetic;

    /**
     * The spin quantum number is the electron spin (s)
     * s = -1/2, 1/2
     */
    quantum_spin spin;
};

struct atom {
    struct electron_orbit *electrons;
    unsigned char electrons_count;
};

struct sim_ctx {
    struct atom *atom;

    struct record_handler *record_handler;

    struct iter_ctx *iter_ctx;

    // Simulation control
    float revolutions;
    bool delta_psi_mode;
    
    scalar time_interval;
    unsigned short record_interval;
};

#endif // ATOM_BOHR_SIM_H