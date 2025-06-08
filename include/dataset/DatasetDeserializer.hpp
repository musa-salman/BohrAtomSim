#pragma once

#include <memory>

#include "Dataset.hpp"

namespace dataset {

class DatasetDeserializer {
  public:
    [[nodiscard]] static std::unique_ptr<Dataset>
    deserialize(const std::string &filename);
};
} // namespace dataset
