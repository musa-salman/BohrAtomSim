#include <stdio.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "utils/iterator.h"
#include "utils/macros.h"

void record2bin(FILE *bin_file, const struct sim_itr *iter, scalar time) {
    double data[] = {C2D(time),        C2D(RHO(iter)),
                     C2D(R_DOT(iter)), C2D(R_DOT_DOT(iter)),
                     C2D(PHI(iter)),   C2D(PHI_DOT(iter)),
                     C2D(GAMMA(iter))};

    fwrite(data, sizeof(double), 7, bin_file);
}