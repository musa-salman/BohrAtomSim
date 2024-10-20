#include <stdio.h>
#include <time.h>


#include "config.h"
#include "utils/constants.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"


void start_iteration(struct sim_ctx *ctx) { ctx->start_time = clock(); }

void end_iteration(struct sim_ctx *ctx) {
    ctx->end_time = clock();
    printf("\n\nfinished calculation for orbit N = %hi , K = %hi , M = %hi "
           "time = %f s \n\n",
           ctx->electron_orbit->principal, ctx->electron_orbit->angular,
           ctx->electron_orbit->magnetic,
           (double)(ctx->end_time - ctx->start_time) / CLOCKS_PER_SEC);
}

void init_iteration(struct sim_itr *itr, enum sim_type type) {
    *itr = (struct sim_itr){.dt = 0,
                            .r = 0,
                            .r_dot = 0,
                            .r_dot_dot = 0,
                            .phi = 0,
                            .phi_dot = 0,
                            // REL
                            .gamma = -1,
                            .delta_phi = 0,
                            // 3D
                            .theta = -1,
                            .theta_dot = -1,
                            .theta_dot_dot = -1,
                            // spin
                            .epsilon = -1,
                            .initial_phi_dot = -1};

    if (type == REL_POLAR || type == REL_SPHERICAL || type == REL_SPIN) {
        itr->gamma = 0;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        itr->theta = 0;
        itr->theta_dot = 0;
        itr->theta_dot_dot = 0;
    }

    if (type == SPIN || type == REL_SPIN) {
        itr->epsilon = 0;
        itr->initial_phi_dot = 0;
    }
}

bool iterate(struct sim_ctx *ctx) {
    const struct sim_itr *curr_itr = &ctx->curr_itr;
    struct sim_itr *next_itr = &ctx->next_itr;
    const struct config *config = ctx->config;
    enum sim_type type = ctx->config->type;

    next_itr->dt += TIME_INTERVAL(config);
    next_itr->r = R_DOT(curr_itr) + R_DOT(curr_itr) * TIME_INTERVAL(config);
    next_itr->r_dot =
        R_DOT(curr_itr) + R_DOT_DOT(curr_itr) * TIME_INTERVAL(config);
    next_itr->phi = PHI(curr_itr) + PHI_DOT(curr_itr) * TIME_INTERVAL(config);

    if (PHI(next_itr) > _2_PI) {
        next_itr->phi -= _2_PI;
    }

    if (type == SPHERICAL || type == REL_SPHERICAL || type == SPIN) {
        next_itr->theta =
            THETA(curr_itr) + THETA_DOT(curr_itr) * TIME_INTERVAL(config);
        next_itr->theta_dot = THETA_DOT(curr_itr) +
                              THETA_DOT_DOT(curr_itr) * TIME_INTERVAL(config);
    }

    return R_DOT(next_itr) * R_DOT(curr_itr) < 0;
}

void log_iteration(FILE *result_f, const struct sim_itr *itr) {
    fprintf(result_f, "t= %LE\t", DT(itr));

    fprintf(result_f, "r= %LE\t", R(itr));

    fprintf(result_f, "r'= %LE\t", R_DOT(itr));
    fprintf(result_f, "r''= %LE\t", R_DOT_DOT(itr));

    fprintf(result_f, "phi= %LE\t", PHI(itr));
    fprintf(result_f, "phi'= %LE\t", PHI_DOT(itr));

    if (PHI_DOT_0(itr) != -1.0) {
        fprintf(result_f, "phi'_0= %LE\t", PHI_DOT_0(itr));
    }
    if (THETA(itr) != -1.0) {
        fprintf(result_f, "theta= %LE\t", THETA(itr));
    }
    if (THETA_DOT(itr) != -1.0) {
        fprintf(result_f, "theta'= %LE\t", THETA_DOT(itr));
    }
    if (THETA_DOT_DOT(itr) != -1.0) {
        fprintf(result_f, "theta''= %LE\t", THETA_DOT_DOT(itr));
    }
    if (GAMMA(itr) != -1.0) {
        fprintf(result_f, "gamma= %LE\t", GAMMA(itr));
    }
    if (DELTA_PHI(itr) != -1.0) {
        fprintf(result_f, "deltaPhi= %LE\t", DELTA_PHI(itr));
    }
    if (EPSILON(itr) != -1.0) {
        fprintf(result_f, "epsilon= %LE\t", EPSILON(itr));
    }

    fprintf(result_f, "\n");
}
