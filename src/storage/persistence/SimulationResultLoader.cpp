#include <algorithm>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "physics/math/types.hpp"
#include "storage/format/SimulationHeaderWriter.hpp"
#include "storage/persistence/SimulationResultLoader.hpp"

namespace storage::persistence {
using namespace storage::format;
using scalar = physics::math::scalar;

template <typename T>
size_t
appendDataRows(std::ifstream &file, size_t start_row, size_t rows_per_chunk,
               size_t field_count, const std::vector<std::string> &field_names,
               std::unordered_map<std::string, std::vector<double>> &datasets);

size_t SimulationResultLoader::loadSimulation(
    const std::string &filepath,
    std::unordered_map<std::string, std::vector<double>> &datasets,
    size_t start_row, size_t rows_per_chunk) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << " ("
                  << strerror(errno) << ")\n";
        return 0;
    }

    RtSimulationHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!file || std::memcmp(header.magic, "ATOM\x89", 5) != 0) {
        std::cerr << "Invalid file header magic.\n";
        return 0;
    }

    std::vector<std::string> field_names;
    for (size_t i = 0; i < header.fieldCount; ++i) {
        char name_buf[MAX_FIELD_NAME] = {0};
        file.read(name_buf, MAX_FIELD_NAME);
        field_names.emplace_back(name_buf);
    }

    size_t rows_added = 0;

    rows_added =
        appendDataRows<double>(file, start_row, rows_per_chunk,
                               header.fieldCount, field_names, datasets);
    file.close();

    return rows_added;
}

template <typename T>
size_t
appendDataRows(std::ifstream &file, size_t start_row, size_t rows_per_chunk,
               size_t field_count, const std::vector<std::string> &field_names,
               std::unordered_map<std::string, std::vector<double>> &datasets) {

    size_t type_size = sizeof(T);
    file.seekg(0, std::ios::end);
    size_t total_bytes = static_cast<size_t>(file.tellg());
    size_t header_bytes =
        sizeof(RtSimulationHeader) + field_count * MAX_FIELD_NAME;
    size_t total_data_bytes = total_bytes - header_bytes;
    size_t total_rows = total_data_bytes / (field_count * type_size);

    if (start_row >= total_rows) {
        for (const std::string &field : field_names)
            datasets[field].clear();

        return 0;
    }

    file.seekg(header_bytes + start_row * field_count * type_size,
               std::ios::beg);

    std::vector<uint8_t> buffer(rows_per_chunk * field_count * type_size);
    size_t total_added = 0;

    while (start_row < total_rows) {
        size_t rows_to_read = std::min(rows_per_chunk, total_rows - start_row);
        size_t chunk_bytes = rows_to_read * field_count * type_size;

        file.read(reinterpret_cast<char *>(buffer.data()), chunk_bytes);
        std::streamsize bytes_read = file.gcount();
        if (static_cast<size_t>(bytes_read) < chunk_bytes) {
            size_t actual_rows = bytes_read / (field_count * type_size);
            if (actual_rows == 0)
                break;
            rows_to_read = actual_rows;
        }

        const T *typed_data = reinterpret_cast<const T *>(buffer.data());

        for (const std::string &field : field_names) {
            datasets[field].reserve(rows_to_read);
        }

        for (size_t row = 0; row < rows_to_read; ++row) {
            for (size_t col = 0; col < field_count; ++col) {
                T val = typed_data[row * field_count + col];
                datasets[field_names[col]].push_back(static_cast<scalar>(val));
            }
        }

        start_row += rows_to_read;
        total_added += rows_to_read;
    }

    return total_added;
}

} // namespace storage::persistence