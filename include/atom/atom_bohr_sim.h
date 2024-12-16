#ifdef __cplusplus
extern "C" {
#endif

#ifndef ATOM_BOHR_SIM_H
#define ATOM_BOHR_SIM_H

#include <stdbool.h>

#include "result/result.h"
#include "utils/iterator.h"
#include "utils/types.h"

#define DEBUG

#ifdef DEBUG
#define INFO(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define INFO(fmt, ...)
#endif

typedef void (*record_fn)(const struct sim_itr *record);

struct record_handler {
    void *record_in;
    record_fn record;
    size_t id;
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

static inline long orbit_hash(struct electron_orbit orbit) {
    return orbit.principal | orbit.angular << 8 | orbit.magnetic << 16 |
           orbit.spin << 24;
}

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

#endif // ATOM_BOHR_SIM_H

#ifdef __cplusplus
}
#endif