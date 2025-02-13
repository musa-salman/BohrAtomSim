#ifdef __cplusplus
extern "C" {
#endif

#ifndef RESULT_RECORDER_H
#define RESULT_RECORDER_H

#include <stdio.h>

#include "utils/iterator.h"

void record2bin(FILE *bin_file, const struct sim_itr *iter);

#endif // RESULT_RECORDER_H

#ifdef __cplusplus
}
#endif