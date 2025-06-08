#pragma once

#include "Dataset.hpp"

namespace dataset {
struct DatasetSerializer {
    static void serialize(const Dataset &dataset, const std::string &filename);
};
} // namespace dataset
