#ifndef DATASET_HPP
#define DATASET_HPP

#include <cstdint>
#include <string>
#include <vector>

#define MAX_FIELD_NAME 16
#define ATOM_MAGIC 0x44535444
#define ATOM_VERSION 1

namespace dataset {

struct DatasetHeader {
    const uint32_t magic = ATOM_MAGIC;
    const uint8_t version = ATOM_VERSION;
    uint8_t columnCount = 0;
    uint8_t rowCount = 0;
};

struct Dataset {
    virtual ~Dataset() = default;
    [[nodiscard]] virtual const std::vector<double> &
    get(std::string_view fieldName) const = 0;
    [[nodiscard]] virtual size_t getRowCount() const = 0;
    [[nodiscard]] virtual size_t getColumnCount() const = 0;
    [[nodiscard]] virtual const std::vector<std::string> &
    getColumnsNames() const = 0;
    [[nodiscard]] const auto &getData() const;
};
} // namespace dataset

#endif // DATASET_HPP