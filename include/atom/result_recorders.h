#ifdef __cplusplus
extern "C" {
#endif

#ifndef RESULT_RECORDER_H
#define RESULT_RECORDER_H

#include <stdio.h>

#include "utils/iterator.h"

#define MAX_FIELD_NAME 16

struct file_header {
    uint8_t magic[5];
    uint8_t version;
    uint8_t field_count;
    uint8_t endianness;
    uint8_t scalar_type;
} __attribute__((packed));

static inline uint8_t detect_endianness() {
    uint16_t x = 0x0123;
    return *((uint8_t *)&x) == 0x23 ? 0 : 1; // 0 = little, 1 = big
}

static inline uint8_t detect_scalar_type() {
#ifdef FLOAT_SCALAR
    return 1;
#elif defined(LONG_DOUBLE_SCALAR)
    return 3;
#else
    return 2; // default: double
#endif
}

static inline void
init_file_header(FILE *bin_file, const uint8_t (*field_names)[MAX_FIELD_NAME],
                 size_t field_count) {
    struct file_header header = {
        .magic = {'A', 'T', 'O', 'M', 0x89},
        .version = 1,
        .field_count = 6,
        .endianness = detect_endianness(),
        .scalar_type = detect_scalar_type(),
    };

    fwrite(&header, sizeof(struct file_header), 1, bin_file);

    for (size_t i = 0; i < field_count; i++) {
        fwrite(field_names[i], sizeof(uint8_t), MAX_FIELD_NAME, bin_file);
    }
}

void record2bin(FILE *bin_file, const struct sim_itr *iter);

#endif // RESULT_RECORDER_H

#ifdef __cplusplus
}
#endif