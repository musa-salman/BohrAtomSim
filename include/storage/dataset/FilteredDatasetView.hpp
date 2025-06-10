#pragma once

#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <vector>

namespace storage::dataset {

struct Range {
    size_t start;
    size_t end; // exclusive

    inline size_t size() const noexcept { return end - start; }
};

class FilteredDatasetView {
  public:
    explicit FilteredDatasetView(size_t rowCount = 0);

    void updateMask(const boost::dynamic_bitset<> &mask);

    void includeAllRows(size_t totalRowCount);

    [[nodiscard]] std::optional<size_t>
    mapFilteredIndex(size_t filteredIndex) const;

    [[nodiscard]] const std::vector<Range> &ranges() const;

    [[nodiscard]] size_t rowCount() const;

  private:
    size_t m_rowCount;
    std::vector<Range> m_ranges;
    std::vector<size_t> m_rangeRowOffsets;
};

} // namespace storage::dataset
