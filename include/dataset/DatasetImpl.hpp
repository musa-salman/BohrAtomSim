#pragma once

#include <unordered_map>

#include "Dataset.hpp"

namespace dataset {

class DatasetImpl : public Dataset {
    std::vector<std::string> fieldNames;
    std::unordered_map<std::string, std::vector<double>> data;

  public:
    const std::vector<double> &get(std::string_view fieldName) const override;

    size_t getRowCount() const override;

    size_t getColumnCount() const override;

    const std::vector<std::string> &getColumnsNames() const override;

    const auto &getData() const { return data; }

    friend class DatasetFactory;
    friend class DatasetSerializer;
    friend class DatasetDeserializer;
};
} // namespace dataset