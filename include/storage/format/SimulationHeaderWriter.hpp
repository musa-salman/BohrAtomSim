#pragma once

#include <array>
#include <span>
#include <stdint.h>
#include <stdio.h>
#include <string_view>

namespace storage::format {

constexpr size_t MAX_FIELD_NAME = 16;

struct RtSimulationHeader {
    uint8_t magic[5];
    uint8_t version;
    uint8_t fieldCount;
    uint8_t endianness;
} __attribute__((packed));

inline uint8_t detectEndianness() {
    uint16_t x = 0x0123;
    return *((uint8_t *)&x) == 0x23 ? '<' : '>';
}

inline void
writeSimulationHeader(FILE *bin_file,
                      std::span<const std::string_view> field_names) {
    RtSimulationHeader header = {
        .magic = {'A', 'T', 'O', 'M', 0x89},
        .version = 1,
        .fieldCount = static_cast<uint8_t>(field_names.size()),
        .endianness = detectEndianness(),
    };

    fwrite(&header, sizeof(header), 1, bin_file);

    for (const auto &name : field_names) {
        std::string_view truncated = name.substr(0, MAX_FIELD_NAME);
        fwrite(truncated.data(), 1, truncated.size(), bin_file);
        if (truncated.size() < MAX_FIELD_NAME) {
            std::array<char, MAX_FIELD_NAME> pad{};
            fwrite(pad.data(), 1, MAX_FIELD_NAME - truncated.size(), bin_file);
        }
    }
}

} // namespace storage::format
