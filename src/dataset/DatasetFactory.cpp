
#include "dataset/DatasetFactory.hpp"
#include "dataset/DatasetImpl.hpp"

std::unique_ptr<Dataset> DatasetFactory::create(
    std::unordered_map<std::string, std::vector<double>> &&data_) {
    auto dataset = std::make_unique<DatasetImpl>();
    dataset->data = std::move(data_);
    for (const auto &entry : dataset->data) {
        dataset->fieldNames.push_back(entry.first);
    }
    return dataset;
}