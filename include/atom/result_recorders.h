#ifndef RESULT_RECORDER_H
#define RESULT_RECORDER_H

#include "utils/iterator.h"

void record2py_list(void *record_in, const unsigned char orbit_i,
                           
                           const struct sim_itr *iter);

#define print(x) _Generic((x), \
    float: printf("%s: %.10E\t", #x, (float)(x)), \
    double: printf("%s: %.10E\t", #x, (double)(x)), \
    long double: printf("%s: %.10LE\t", #x, (long double)(x)) \
)

void record2printf(void *record_in, const unsigned char orbit_i,
                           const struct sim_itr *iter);

#endif