#ifdef __cplusplus
extern "C" {
#endif

#ifndef ATOM_BOHR_SIM_H
#define ATOM_BOHR_SIM_H

#include <stdbool.h>
#include <stdio.h>

#include "utils/types.h"

#define DEBUG

#define C2D(x)                                                                 \
    _Generic((x), float: ((double)(x)), double: (x), long double: ((double)(x)))

#ifdef DEBUG
#define INFO(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define INFO(fmt, ...)
#endif

struct electron_orbit {
    /**
     * The principal quantum number is orbital size and energy level (n)
     * n = 1, 2, 3,...
     */
    quantum_principle principal;

    /**
     * The azimuthal quantum number is the orbital shape aka subshell
     * (l) l = 0, 1, 2,..., n-1
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

struct sim_ctx {
    size_t id;

    unsigned short record_interval;
    // Simulation control
    float revolutions;

    scalar time_interval;

    struct electron_orbit orbit;
};

#endif // ATOM_BOHR_SIM_H

#ifdef __cplusplus
}
#endif