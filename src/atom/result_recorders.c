#include <stdio.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "utils/iterator.h"
#include "utils/macros.h"

void record2bin(FILE *bin_file, const struct sim_itr *iter, scalar time) {
    // Prepare data buffer
    double data[] = {C2D(time),        C2D(RHO(iter)),
                     C2D(R_DOT(iter)), C2D(R_DOT_DOT(iter)),
                     C2D(PHI(iter)),   C2D(PHI_DOT(iter))};

    // Write the fixed data
    fwrite(data, sizeof(double), 6, bin_file);

    if (THETA(iter) != -1) {
        double theta_data[] = {C2D(THETA(iter)), C2D(THETA_DOT(iter)),
                               C2D(THETA_DOT_DOT(iter))};
        fwrite(theta_data, sizeof(double), 3, bin_file);
    }

    if (GAMMA(iter) != -1) {
        double gamma_data[] = {C2D(GAMMA(iter)), C2D(DELTA_PHI(iter))};
        fwrite(gamma_data, sizeof(double), 2, bin_file);
    }
}