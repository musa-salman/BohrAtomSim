#ifndef ATOM_BOHR_SIM_H
#define ATOM_BOHR_SIM_H

#include <stdbool.h>

#include "utils/iterator.h"
#include "utils/types.h"
#include "result/result.h"

#ifdef DEBUG
#define INFO(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define INFO(fmt, ...)
#endif

typedef void (*record_fn)(void *record_in, const struct sim_itr* record);
typedef void *(*records_lookup_fn)(void *records_map, long key);

struct record_handler {
    void *record_in;
    record_fn record;
    records_lookup_fn records_lookup;
    unsigned char curr_records;
};

#define RECORD_ITERATION(sim_ctx, record_in, curr_itr)                         \
    (sim_ctx)->record_handler->record((record_in), (curr_itr))

#define ORBIT_RECORD_LOCATION(record_handler, key)                             \
    (record_handler)->records_lookup((record_handler)->record_in, (key))

struct electron_orbit {
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

long orbit_hash(struct electron_orbit orbit);

struct atom {
    struct electron_orbit *electrons;
    unsigned char electrons_count;
};

struct sim_ctx {
    struct record_handler *record_handler;

    // Simulation control
    float revolutions;

    // TODO: move to recorder handler
    bool delta_psi_mode;

    scalar time_interval;

    // TODO: move to recorder handler
    unsigned short record_interval;

    struct result *result;
    // parallelism
    unsigned char active_orbits;
};

void reader_function(struct sim_ctx *ctx);

#endif // ATOM_BOHR_SIM_H