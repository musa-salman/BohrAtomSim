#pragma once

#include <memory>
#include <unordered_map>

#include "Dataset.hpp"
#include "FilteredDatasetView.hpp"

namespace storage::dataset {

class DatasetFactory {
  public:
    [[nodiscard]] static std::unique_ptr<Dataset>
    create(std::unordered_map<std::string, std::vector<double>> &&data_);

    [[nodiscard]] static std::unique_ptr<Dataset>
    create(const Dataset &dataset, const FilteredDatasetView &view);
};
} // namespace storage::dataset