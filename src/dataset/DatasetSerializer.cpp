
#include <cstdint>
#include <fstream>

#include "dataset/DatasetSerializer.hpp"

void DatasetSerializer::serialize(const Dataset &dataset,
                                  const std::string &filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out)
        throw std::runtime_error("Failed to open file");

    DatasetHeader header;
    header.columnCount = dataset.getColumnCount();
    header.rowCount = dataset.getRowCount();

    out.write(reinterpret_cast<const char *>(&header), sizeof(header));

    for (const auto &name : dataset.getColumnsNames()) {
        uint64_t len = name.size();
        out.write(reinterpret_cast<const char *>(&len), sizeof(len));
        out.write(name.data(), len);

        const auto &column = dataset.get(name);
        out.write(reinterpret_cast<const char *>(column.data()),
                  sizeof(double) * column.size());
    }
}