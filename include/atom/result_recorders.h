#ifdef __cplusplus
extern "C" {
#endif

#ifndef RESULT_RECORDER_H
#define RESULT_RECORDER_H

#include "utils/iterator.h"

void record2csv(void *record_in, const struct sim_itr *iter);

#endif

#ifdef __cplusplus
}
#endif