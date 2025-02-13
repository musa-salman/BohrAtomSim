#ifdef __cplusplus
extern "C" {
#endif

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>

#define FILE_PATH_SIZE 256 // NOSONAR

inline void format_output_filename(size_t id,
                                   char output_filename[FILE_PATH_SIZE]) {
    snprintf(output_filename, FILE_PATH_SIZE, "%ssimulations/%ld.bin", DB_PATH,
             id);
}

#endif // UTILS_H

#ifdef __cplusplus
}
#endif