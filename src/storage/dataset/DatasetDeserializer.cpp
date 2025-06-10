
#include <fstream>

#include "storage/dataset/DatasetDeserializer.hpp"
#include "storage/dataset/DatasetImpl.hpp"

namespace storage::dataset {

std::unique_ptr<Dataset>
DatasetDeserializer::deserialize(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in)
        throw std::runtime_error("Failed to open file");

    DatasetHeader header;
    in.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.magic != 0x44535444)
        throw std::runtime_error("Invalid file format");

    if (header.version != 1)
        throw std::runtime_error("Unsupported version");

    auto dataset = std::make_unique<DatasetImpl>();

    for (size_t i = 0; i < header.columnCount; ++i) {
        uint64_t len;
        in.read(reinterpret_cast<char *>(&len), sizeof(len));

        std::string name(len, '\0');
        in.read(name.data(), len);

        std::vector<double> columnData(header.rowCount);
        in.read(reinterpret_cast<char *>(columnData.data()),
                sizeof(double) * header.rowCount);

        dataset->fieldNames.push_back(name);
        dataset->data[name] = std::move(columnData);
    }

    return dataset;
}
} // namespace storage::dataset