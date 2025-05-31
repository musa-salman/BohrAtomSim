#ifndef RESULT_RECORDER_HPP
#define RESULT_RECORDER_HPP

#include <array>
#include <span>
#include <stdint.h>
#include <stdio.h>
#include <string_view>

#include "utils/types.h"

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
init_file_header(FILE *bin_file,
                 std::span<const std::string_view> field_names) {
    struct file_header header = {
        .magic = {'A', 'T', 'O', 'M', 0x89},
        .version = 1,
        .field_count = static_cast<uint8_t>(field_names.size()),
        .endianness = detect_endianness(),
    };

    fwrite(&header, sizeof(file_header), 1, bin_file);

    size_t field_count = field_names.size();
    for (size_t i = 0; i < field_count; i++) {
        std::string_view name = field_names[i];
        if (name.size() > MAX_FIELD_NAME) {
            name = name.substr(0, MAX_FIELD_NAME);
        }
        fwrite(name.data(), sizeof(char), name.size(), bin_file);
        size_t padding = MAX_FIELD_NAME - name.size();
        if (padding > 0) {
            std::array<char, MAX_FIELD_NAME> pad{};
            fwrite(pad.data(), sizeof(char), padding, bin_file);
        }
    }
}

#endif // RESULT_RECORDER_HPP