#ifndef FILTERED_DATASET_VIEW_HPP
#define FILTERED_DATASET_VIEW_HPP

#include <unordered_map>

#include "dataset/Dataset.hpp"
#include "dataset/MaskedColumnCache.hpp"

class FilteredDatasetView {
    const Dataset &baseDataset;
    const std::vector<uint8_t> *mask = nullptr;

    mutable std::unordered_map<std::string, MaskedColumnCache> cache;

  public:
    explicit FilteredDatasetView(const Dataset &dataset);

    void setMask(const std::vector<uint8_t> &newMask);

    [[nodiscard]] const std::vector<std::string> &getColumns() const;

    [[nodiscard]] const std::vector<double> &get(std::string_view column) const;

    [[nodiscard]] size_t getRowCount() const;

    size_t getColumnCount() const { return baseDataset.getColumnCount(); }
};

#endif // FILTERED_DATASET_VIEW_HPP
