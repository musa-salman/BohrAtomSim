#pragma once

#include "Dataset.hpp"

namespace storage::dataset {
struct DatasetSerializer {
    static void serialize(const Dataset &dataset, const std::string &filename);
};
} // namespace storage::dataset
