#include <string.h>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"

#include "polar/polar_sim.h"

#include "atom/result_recorders.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"
#include "utils/utils.h"

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar time_interval,
                                quantum_angular angular);

void simulate_polar_orbit(const struct sim_ctx ctx) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};
    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};
    struct electron_orbit orbit = ctx.orbit;

    char file_name[FILE_PATH_SIZE];
    format_output_filename(ctx.id, file_name);

    FILE *file_bin = fopen(file_name, "wb");
    if (file_bin == NULL) {
        return;
    }

    {
        const uint8_t field_names_2DNR[][MAX_FIELD_NAME] = {
            "t", "r", "r_dot", "r_ddot", "phi", "phi_dot",
        };

        init_file_header(file_bin, field_names_2DNR, 6);
    }
    start_iteration(&iter_ctx);

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    init_iteration(&prev_itr, POLAR);
    init_iteration(&next_itr, POLAR);

    prev_itr.r = radial_bounds.r_min;
    prev_itr.r_dot_dot = compute_r_dot_dot(prev_itr.r, orbit.angular);
    prev_itr.phi_dot = POLAR_PHI_DOT(orbit.angular, prev_itr.r);

    scalar revolutions = ctx.revolutions;

    size_t it = 0;
    while (revolutions > 0) {
        const bool is_max =
            simulate_orbit_step(&iter_ctx, ctx.time_interval, orbit.angular);

        if (it % ctx.record_interval == 0)
            record2bin(file_bin, &prev_itr, iter_ctx.time);
        if (is_max) {
            revolutions = revolutions - 0.5;
            if (revolutions <= 0)
                break;
        }

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }

    fclose(file_bin);
    end_iteration(&iter_ctx);
}

static bool simulate_orbit_step(struct iter_ctx *iter_ctx, scalar time_interval,
                                quantum_angular angular) {
    const struct sim_itr *prev_itr = iter_ctx->prev_itr;
    struct sim_itr *next_itr = iter_ctx->next_itr;

    const bool is_at_interest = iterate(iter_ctx, time_interval, POLAR);

    next_itr->r_dot_dot = compute_r_dot_dot(RHO(prev_itr), angular);
    next_itr->phi_dot = POLAR_PHI_DOT(angular, RHO(next_itr));

    return is_at_interest;
}