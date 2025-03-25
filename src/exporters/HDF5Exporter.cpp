#include <H5Cpp.h>
#include <iostream>
#include <stdexcept>

#include "H5PredType.h"
#include "exporters/HDF5Exporter.hpp"

void HDF5Exporter::exportData(
    const std::string &path,
    const std::unordered_map<std::string, std::vector<double>> &datasets) {

    try {
        H5::H5File file(path, H5F_ACC_TRUNC);
        for (const auto &pair : datasets) {
            std::cout << pair.first << " ";
        }
        std::cout << std::endl;

        for (const auto &pair : datasets) {
            const std::string &name = pair.first;
            const std::vector<double> &data = pair.second;

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
