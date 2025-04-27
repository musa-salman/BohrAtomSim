#include <string.h>

#include "atom/atom_bohr_sim.h"

#include "polar/polar_sim_rel.h"

#include "atom/result_recorders.h"
#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"
#include "utils/utils.h"

inline static bool simulate_orbit_step(struct iter_ctx *ctx,
                                       scalar time_interval, scalar angular);

void simulate_2d_electron_motion(const struct sim2d_ctx ctx) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};
    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};

    char file_name[FILE_PATH_SIZE];
    format_output_filename(ctx.id, file_name);

    FILE *file_bin = fopen(file_name, "wb");
    if (file_bin == NULL) {
        return;
    }

    scalar prev_max_vec = 0;

    {
        const uint8_t field_names_2DR[][MAX_FIELD_NAME] = {
            "t", "r", "r_dot", "r_ddot", "phi", "phi_dot", "gamma"};

        init_file_header(file_bin, field_names_2DR,
                         sizeof(field_names_2DR) / sizeof(field_names_2DR[0]));
    }
    init_iteration(&prev_itr, REL_POLAR);
    init_iteration(&next_itr, REL_POLAR);

    const scalar angular_momentum = ctx.v_0 * ctx.r_0 * sim_sin(ctx.theta_rv);

    prev_itr.r = ctx.r_0;

    prev_itr.gamma =
        compute_gamma(angular_momentum, prev_itr.r, prev_itr.r_dot);

    prev_itr.r_dot_dot = compute_rel_r_ddot(angular_momentum, prev_itr.gamma,
                                            prev_itr.r, prev_itr.r_dot);

    prev_itr.phi_dot =
        POLAR_PHI_DOT_REL(angular_momentum, prev_itr.r, prev_itr.gamma);

    bool is_at_maximum = true;
    bool is_at_interest = false;

    size_t it = 0;
    while (iter_ctx.time < ctx.total_duration) {
        is_at_interest =
            simulate_orbit_step(&iter_ctx, ctx.time_interval, angular_momentum);

        if (it % ctx.record_interval == 0)
            record2bin(file_bin, &prev_itr, iter_ctx.time);

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }

    fclose(file_bin);
    end_iteration(&iter_ctx);
}

inline static bool simulate_orbit_step(struct iter_ctx *ctx,
                                       scalar time_interval, scalar angular) {
    const struct sim_itr *prev_itr = ctx->prev_itr;
    struct sim_itr *next_itr = ctx->next_itr;

    const bool is_at_interest = iterate(ctx, time_interval, REL_POLAR);

    next_itr->r_dot_dot = compute_rel_r_ddot(angular, GAMMA(prev_itr),
                                             RHO(prev_itr), R_DOT(prev_itr));

    next_itr->phi_dot =
        POLAR_PHI_DOT_REL(angular, RHO(prev_itr), GAMMA(prev_itr));

    next_itr->gamma = compute_gamma(angular, RHO(prev_itr), R_DOT(prev_itr));

    return is_at_interest;
}