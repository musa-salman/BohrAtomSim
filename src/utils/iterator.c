#include <stdbool.h>
#include <time.h>

#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

void start_iteration(struct iter_ctx *ctx) { ctx->start_time = clock(); }

void end_iteration(struct iter_ctx *ctx) { ctx->end_time = clock(); }

void init_iteration(struct sim_itr *itr, enum sim_type type) {
    *itr = (struct sim_itr){
        .dt = 0,
        .r = 0,
        .r_dot = 0,
        .r_dot_dot = 0,

        .phi = 0,
        .phi_dot = 0,
        .phi_dot_dot = 0,

        // 3D
        .theta = -1,
        .theta_dot = -1,
        .theta_dot_dot = -1,

        // REL
        .delta_phi = -1,
        // spin
        .gamma = -1,
        .epsilon = -1,
    };

    if (type == REL_POLAR || type == REL_SPHERICAL || type == REL_SPIN) {
        itr->gamma = 0;
        itr->delta_phi = 0;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        itr->theta = 0;
        itr->theta_dot = 0;
        itr->theta_dot_dot = 0;
    }

    if (type == SPIN || type == REL_SPIN) {
        itr->epsilon = 0;
    }
}