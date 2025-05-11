#ifndef FILTERED_DATASET_VIEW_HPP
#define FILTERED_DATASET_VIEW_HPP

#include <memory>
#include <unordered_map>

#include "dataset/BitVector.hpp"
#include "dataset/Dataset.hpp"
#include "dataset/MaskedColumnCache.hpp"

class FilteredDatasetView {
    const Dataset *baseDataset = nullptr;
    const BitVector *mask = nullptr;

    mutable std::unordered_map<std::string, MaskedColumnCache> cache;

  public:
    void setBaseDataset(const Dataset &newBaseDataset);
    void setMask(const BitVector &newMask);

    [[nodiscard]] std::unique_ptr<Dataset> toDataset();

    [[nodiscard]] const std::vector<std::string> &getColumns() const;

    [[nodiscard]] const std::vector<double> &get(std::string_view column) const;

    [[nodiscard]] size_t getRowCount() const;

    size_t getColumnCount() const { return baseDataset->getColumnCount(); }
};

#endif // FILTERED_DATASET_VIEW_HPP
