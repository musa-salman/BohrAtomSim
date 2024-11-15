#ifndef RESULT_RECORDER_H
#define RESULT_RECORDER_H

#include "utils/iterator.h"

void record2py_list(void *record_in, const struct sim_itr*  record);

void record2py_list_verbose(void *record_in, const struct sim_itr *iter);

void record2py_list_rel(void *record_in, const struct sim_itr*  record);

#define print(x) _Generic((x), \
    float: printf("%s: %.5E\t", #x, (float)(x)), \
    double: printf("%s: %.5E\t", #x, (double)(x)), \
    long double: printf("%s: %.5LE\t", #x, (long double)(x)) \
)

void record2printf(void *record_in, const struct sim_itr *iter);

#endif