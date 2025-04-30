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
} __attribute__((packed));

static inline uint8_t detect_endianness() {
    uint16_t x = 0x0123;
    return *((uint8_t *)&x) == 0x23 ? '<' : '>';
}

static inline void
init_file_header(FILE *bin_file, const uint8_t (*field_names)[MAX_FIELD_NAME],
                 uint8_t field_count) {
    struct file_header header = {
        .magic = {'A', 'T', 'O', 'M', 0x89},
        .version = 1,
        .field_count = field_count,
        .endianness = detect_endianness(),
    };

    fwrite(&header, sizeof(struct file_header), 1, bin_file);

    for (size_t i = 0; i < field_count; i++) {
        fwrite(field_names[i], sizeof(uint8_t), MAX_FIELD_NAME, bin_file);
    }
}

void record2bin(FILE *bin_file, const struct sim_itr *iter, scalar time);

#endif // RESULT_RECORDER_H

#ifdef __cplusplus
}
#endif