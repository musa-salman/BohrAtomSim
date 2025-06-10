#pragma once

#include <memory>

#include "Dataset.hpp"

namespace storage::dataset {

class DatasetDeserializer {
  public:
    [[nodiscard]] static std::unique_ptr<Dataset>
    deserialize(const std::string &filename);
};
} // namespace storage::dataset
