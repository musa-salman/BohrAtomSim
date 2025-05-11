
#include "dataset/FilteredDatasetView.hpp"
#include "dataset/BitVector.hpp"
#include "dataset/Dataset.hpp"
#include "dataset/DatasetFactory.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

void FilteredDatasetView::setBaseDataset(const Dataset &newBaseDataset) {
    baseDataset = &newBaseDataset;

    for (auto &[_, columnCache] : cache)
        columnCache.setSource(nullptr, nullptr);
}

void FilteredDatasetView::setMask(const BitVector &newMask) {
    mask = &newMask;

    for (auto &[_, columnCache] : cache)
        columnCache.setSource(nullptr, nullptr);
}

const std::vector<std::string> &FilteredDatasetView::getColumns() const {
    return baseDataset->getColumnsNames();
}

const std::vector<double> &
FilteredDatasetView::get(std::string_view column) const {
    if (!mask)
        throw std::runtime_error("Mask not set");

    auto &cacheEntry = cache[std::string(column)];

    if (cacheEntry.getSource() == nullptr)
        cacheEntry.setSource(baseDataset->get(column), *mask);

    return cacheEntry.get();
}

std::unique_ptr<Dataset> FilteredDatasetView::toDataset() {
    if (!mask)
        throw std::runtime_error("Mask not set");

    std::unordered_map<std::string, std::vector<double>> filteredData;

    for (const auto &column : baseDataset->getColumnsNames())
        filteredData[column] = get(column);

    return DatasetFactory::create(std::move(filteredData));
}

size_t FilteredDatasetView::getRowCount() const {
    if (!mask)
        return 0;
    size_t count = 0;
    for (size_t i = 0; i < mask->bitSize(); ++i) {
        if (mask->get(i)) {
            ++count;
        }
    }

    return count;
}