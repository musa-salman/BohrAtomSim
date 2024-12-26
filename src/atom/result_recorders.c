#include <stdio.h>

#include "atom/result_recorders.h"
#include "utils/iterator.h"
#include "utils/macros.h"

void record2csv(void *record_in, const struct sim_itr *iter) {
    FILE *csv = (FILE *)record_in;

    fprintf(csv, "%.20LE,%.20LE,%.20LE,%.20LE,%.20LE,%.20LE", iter->dt,
            R(iter), R_DOT(iter), R_DOT_DOT(iter), PHI(iter), PHI_DOT(iter));

    if (THETA(iter) != -1) {
        fprintf(csv, ",%.20E,%.20E,%.20E", THETA(iter), THETA_DOT(iter),
                THETA_DOT_DOT(iter));
    }

    if (EPSILON(iter) != -1) {
        fprintf(csv, ",%.20E", EPSILON(iter));
    }

    if (GAMMA(iter) != -1) {
        fprintf(csv, ",%.20E,%.20E", GAMMA(iter), DELTA_PHI(iter));
    }

    fprintf(csv, "\n");
}