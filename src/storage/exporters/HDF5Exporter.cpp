#include <H5Cpp.h>
#include <H5PredType.h>
#include <stdexcept>

#include "storage/exporters/HDF5Exporter.hpp"

namespace storage::exporters {

void HDF5Exporter::exportData(const std::string &path,
                              const dataset::Dataset &datasets) {

    try {
        H5::H5File file(path, H5F_ACC_TRUNC);

        for (const auto &name : datasets.getColumnsNames()) {
            const auto &data = datasets.get(name);

            hsize_t dims[1] = {data.size()};
            H5::DataSpace dataspace(1, dims);

            H5::DataSet dataset = file.createDataSet(
                name, H5::PredType::NATIVE_DOUBLE, dataspace);
            dataset.write(data.data(), H5::PredType::NATIVE_DOUBLE);
        }

    } catch (H5::FileIException &e) {
        throw std::runtime_error("Failed to create or write HDF5 file: " +
                                 path);
    } catch (H5::DataSetIException &e) {
        throw std::runtime_error("Failed to write HDF5 dataset.");
    }
}
} // namespace storage::exporters
