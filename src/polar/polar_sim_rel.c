#include <string.h>

#include "atom/atom_bohr_sim.h"

#include "polar/polar_sim_rel.h"

#include "atom/result_recorders.h"
#include "orbital_math.h"
#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"
#include "utils/utils.h"

inline static bool simulate_orbit_rel_step(struct iter_ctx *ctx,
                                           scalar time_interval,
                                           quantum_angular angular);

void simulate_polar_orbit_rel(const struct sim_ctx ctx) {
    struct sim_itr prev_itr = {};
    struct sim_itr next_itr = {};
    struct iter_ctx iter_ctx = {.prev_itr = &prev_itr, .next_itr = &next_itr};
    struct electron_orbit orbit = ctx.orbit;

    struct radial_bounds radial_bounds =
        compute_radial_limits(orbit.principal, orbit.angular);

    char file_name[FILE_PATH_SIZE];
    format_output_filename(ctx.id, file_name);

    FILE *file_bin = fopen(file_name, "wb");
    if (file_bin == NULL) {
        return;
    }

    scalar prev_max_vec = 0;

    {
        const uint8_t field_names_2DR[][MAX_FIELD_NAME] = {
            "t", "r", "r_dot", "r_ddot", "phi", "phi_dot", "gamma", "delta_psi",
        };

        init_file_header(file_bin, field_names_2DR, 8);
    }
    init_iteration(&prev_itr, REL_POLAR);
    init_iteration(&next_itr, REL_POLAR);

    prev_itr.r = radial_bounds.r_min;

    prev_itr.gamma = compute_gamma(orbit.angular, prev_itr.r, prev_itr.r_dot);

    prev_itr.r_dot_dot = compute_rel_r_ddot(orbit.angular, prev_itr.gamma,
                                            prev_itr.r, prev_itr.r_dot);

    prev_itr.phi_dot =
        POLAR_PHI_DOT_REL(orbit.angular, prev_itr.r, prev_itr.gamma);
    scalar revolutions = ctx.revolutions;

    bool is_at_maximum = true;
    bool is_at_interest = false;

    size_t it = 0;
    while (revolutions > 0) {
        is_at_interest = simulate_orbit_rel_step(&iter_ctx, ctx.time_interval,
                                                 orbit.angular);

        if (it % ctx.record_interval == 0)
            record2bin(file_bin, &prev_itr, iter_ctx.time);

        if (is_at_interest) {
            is_at_maximum = !is_at_maximum;
            if (is_at_maximum) {
                iter_ctx.prev_itr->delta_phi +=
                    PHI(iter_ctx.prev_itr) - prev_max_vec;
                record2bin(file_bin, iter_ctx.prev_itr, iter_ctx.time);

                iter_ctx.next_itr->delta_phi = DELTA_PHI(iter_ctx.prev_itr);

                prev_max_vec = PHI(iter_ctx.prev_itr);
            }

            revolutions -= 0.5;
            if (revolutions <= 0) {
                break;
            }
        }

        struct sim_itr *tmp = iter_ctx.prev_itr;
        iter_ctx.prev_itr = iter_ctx.next_itr;

        iter_ctx.next_itr = tmp;
        it++;
    }

    fclose(file_bin);
    end_iteration(&iter_ctx);
}

inline static bool simulate_orbit_rel_step(struct iter_ctx *ctx,
                                           scalar time_interval,
                                           quantum_angular angular) {
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