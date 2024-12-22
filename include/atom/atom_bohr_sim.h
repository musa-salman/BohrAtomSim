#ifdef __cplusplus
extern "C" {
#endif

#ifndef ATOM_BOHR_SIM_H
#define ATOM_BOHR_SIM_H

#include <stdbool.h>

#include "utils/iterator.h"
#include "utils/types.h"

#define DEBUG

#ifdef DEBUG
#define INFO(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define INFO(fmt, ...)
#endif

#define RECORD_ITERATION(sim_ctx, curr_itr)                                    \
    (sim_ctx)->record_handler.record((sim_ctx)->record_handler.record_in,      \
                                     (sim_ctx)->record_handler.name,           \
                                     (curr_itr))

struct record_handler {
    const char *name;
    void *record_in;
    void (*record)(void *record_in, const char *name,
                   const struct sim_itr *sim_itr);

    unsigned short record_interval;
    bool delta_psi_mode;
};

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

struct atom {
    struct electron_orbit *electrons;
    unsigned char electrons_count;
};

struct sim_ctx {
    struct record_handler record_handler;

    // Simulation control
    float revolutions;

    scalar time_interval;
};

#endif // ATOM_BOHR_SIM_H

#ifdef __cplusplus
}
#endif