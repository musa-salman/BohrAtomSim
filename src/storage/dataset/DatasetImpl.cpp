
#include <string>

#include "storage/dataset/DatasetImpl.hpp"

namespace storage::dataset {

const std::vector<double> &DatasetImpl::get(std::string_view fieldName) const {
    return data.at(std::string(fieldName));
}

size_t DatasetImpl::getRowCount() const {
    if (data.empty())
        return 0;
    return data.begin()->second.size();
}

size_t DatasetImpl::getColumnCount() const { return data.size(); }

const std::vector<std::string> &DatasetImpl::getColumnsNames() const {
    return fieldNames;
}
} // namespace storage::dataset
