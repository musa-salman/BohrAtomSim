#ifndef DATASET_FACTORY_HPP
#define DATASET_FACTORY_HPP

#include <memory>
#include <unordered_map>

#include "Dataset.hpp"

class DatasetFactory {
  public:
    [[nodiscard]] static std::unique_ptr<Dataset>
    create(std::unordered_map<std::string, std::vector<double>> &&data_);
};

#endif // DATASET_FACTORY_HPP