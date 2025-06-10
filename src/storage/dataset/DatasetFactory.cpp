
#include "storage/dataset/DatasetFactory.hpp"
#include "storage/dataset/DatasetImpl.hpp"

namespace storage::dataset {

std::unique_ptr<Dataset> DatasetFactory::create(
    std::unordered_map<std::string, std::vector<double>> &&data_) {
    auto dataset = std::make_unique<DatasetImpl>();
    dataset->data = std::move(data_);
    for (const auto &entry : dataset->data) {
        dataset->fieldNames.push_back(entry.first);
    }
    return dataset;
}

std::unique_ptr<Dataset>
DatasetFactory::create(const Dataset &dataset,
                       const FilteredDatasetView &view) {
    std::unordered_map<std::string, std::vector<double>> data;

    const auto &ranges = view.ranges();

    const auto &columnNames = dataset.getColumnsNames();
    for (const auto &name : columnNames) {
        data[name].reserve(view.rowCount());
    }

    for (const auto &range : ranges) {
        for (const auto &name : columnNames) {
            const auto &column = dataset.get(name);
            data[name].insert(data[name].end(),
                              column.begin() + static_cast<long>(range.start),
                              column.begin() + static_cast<long>(range.end));
        }
    }

    return DatasetFactory::create(std::move(data));
}

} // namespace storage::dataset