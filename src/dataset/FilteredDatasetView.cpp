
#include <vector>

#include "dataset/FilteredDatasetView.hpp"

namespace dataset {

FilteredDatasetView::FilteredDatasetView(size_t rowCount)
    : m_rowCount(rowCount) {
    if (rowCount > 0) {
        m_ranges.reserve(10);
        m_ranges.push_back({0, rowCount});
        m_rangeRowOffsets.reserve(10);
        m_rangeRowOffsets.push_back(0);
    }
}

void FilteredDatasetView::updateMask(const boost::dynamic_bitset<> &mask) {
    m_ranges.clear();
    m_rangeRowOffsets.clear();
    m_rowCount = 0;

    m_ranges.reserve(mask.size() / 2);
    m_rangeRowOffsets.reserve(mask.size() / 2);

    size_t i = 0;
    const size_t n = mask.size();
    while (i < n) {
        i = mask.find_next(i);
        if (i == boost::dynamic_bitset<>::npos)
            break;

        size_t start = i;
        while (i < n && mask[i])
            ++i;

        m_ranges.push_back({start, i});
        m_rangeRowOffsets.push_back(m_rowCount);
        m_rowCount += (i - start);
    }

    m_ranges.shrink_to_fit();
    m_rangeRowOffsets.shrink_to_fit();
}

void FilteredDatasetView::includeAllRows(size_t totalRowCount) {
    m_ranges = {{0, totalRowCount}};
    m_rangeRowOffsets = {0};
    m_rowCount = totalRowCount;

    m_ranges.shrink_to_fit();
    m_rangeRowOffsets.shrink_to_fit();
}

std::optional<size_t>
FilteredDatasetView::mapFilteredIndex(size_t filteredIndex) const {
    if (filteredIndex >= m_rowCount)
        return std::nullopt;

    auto it = std::upper_bound(m_rangeRowOffsets.begin(),
                               m_rangeRowOffsets.end(), filteredIndex);

    size_t rangeIdx = std::distance(m_rangeRowOffsets.begin(), it) - 1;

    const auto &r = m_ranges[rangeIdx];
    size_t offset = filteredIndex - m_rangeRowOffsets[rangeIdx];
    return r.start + offset;
}

const std::vector<Range> &FilteredDatasetView::ranges() const {
    return m_ranges;
}

size_t FilteredDatasetView::rowCount() const { return m_rowCount; }
} // namespace dataset
