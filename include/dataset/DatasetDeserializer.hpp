#ifndef DATASET_DESERIALIZER_HPP
#define DATASET_DESERIALIZER_HPP

#include <memory>

#include "Dataset.hpp"

class DatasetDeserializer {
  public:
    [[nodiscard]] static std::unique_ptr<Dataset>
    deserialize(const std::string &filename);
};

#endif // DATASET_DESERIALIZER_HPP