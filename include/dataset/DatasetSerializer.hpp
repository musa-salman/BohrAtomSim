#ifndef DATASET_SERIALIZER_HPP
#define DATASET_SERIALIZER_HPP

#include "Dataset.hpp"

struct DatasetSerializer {
    static void serialize(const Dataset &dataset, const std::string &filename);
};

#endif // DATASET_SERIALIZER_HPP