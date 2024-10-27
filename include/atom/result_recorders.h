#ifndef RESULT_RECORDER_H
#define RESULT_RECORDER_H

#include "utils/types.h"

void record2ndarray(void *record_in, const unsigned char orbit_i,
                           const unsigned char iter_i,
                           const struct sim_itr *iter);

void record2printf(void *record_in, const unsigned char orbit_i,
                           const unsigned char iter_i,
                           const struct sim_itr *iter);

#endif