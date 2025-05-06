#ifndef DATASET_IMPL_HPP
#define DATASET_IMPL_HPP

#include <unordered_map>

#include "Dataset.hpp"

class DatasetImpl : public Dataset {
    std::vector<std::string> fieldNames;
    std::unordered_map<std::string, std::vector<double>> data;

  public:
    const std::vector<double> &get(std::string_view fieldName) const override;

    size_t getRowCount() const override;

    size_t getColumnCount() const override;

    const std::vector<std::string> &getColumns() const override;

    friend class DatasetFactory;
    friend class DatasetSerializer;
    friend class DatasetDeserializer;
};

#endif // DATASET_IMPL_HPP