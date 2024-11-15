#ifndef RESULT_H
#define RESULT_H

#include <stdlib.h>

struct record {
    float r;
    float phi;
    float theta;
    float delta_psi;
};

struct records {
    struct record *records;
    size_t count;
    size_t capacity;
};

struct result {
    struct records *records;
    size_t count;
};

struct result *create_result(size_t count);
void free_result(struct result *result);

void add_record(struct result *records, long orbit_i, struct record record);


#endif // RESULT_H