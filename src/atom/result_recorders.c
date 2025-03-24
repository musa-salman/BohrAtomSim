#include <stdio.h>

#include "atom/result_recorders.h"
#include "utils/iterator.h"
#include "utils/macros.h"

void record2bin(FILE *bin_file, const struct sim_itr *iter) {
    // Prepare data buffer
    scalar data[] = {iter->dt,        RHO(iter), R_DOT(iter),
                     R_DOT_DOT(iter), PHI(iter), PHI_DOT(iter)};

    // Write the fixed data
    fwrite(data, sizeof(scalar), 6, bin_file);

    if (THETA(iter) != -1) {
        scalar theta_data[] = {THETA(iter), THETA_DOT(iter),
                               THETA_DOT_DOT(iter)};
        fwrite(theta_data, sizeof(scalar), 3, bin_file);
    }

    if (GAMMA(iter) != -1) {
        scalar gamma_data[] = {GAMMA(iter), DELTA_PHI(iter)};
        fwrite(gamma_data, sizeof(scalar), 2, bin_file);
    }
}