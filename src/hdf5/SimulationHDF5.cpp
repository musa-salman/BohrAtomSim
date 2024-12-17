#include <H5Cpp.h>
#include <array>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "hdf5/SimulationHDF5.hpp"

SimulationDataWriter::SimulationDataWriter(const std::string &filename) {
    if (std::filesystem::exists(filename)) {
        file = H5::H5File(filename, H5F_ACC_RDWR);
    } else {
        file = H5::H5File(filename, H5F_ACC_TRUNC);
    }

    if (!file.getId()) {
        throw std::ios_base::failure("Failed to open file: " + filename);
    }

    if (!file.attrExists(BASE_GROUP)) {
        file.createGroup(BASE_GROUP);
    }
}

SimulationDataWriter::~SimulationDataWriter() { file.close(); }

void SimulationDataWriter::createSimulationGroup(
    const std::string &group_name,
    const std::unordered_map<std::string, std::string> &meta_data) {
    H5::Group group = file.createGroup(BASE_GROUP + "/" + group_name);

    std::scoped_lock lock(hdf5_mutex);

    for (const auto &[key, value] : meta_data) {
        H5::StrType strType(H5::PredType::C_S1, value.size());
        group.createAttribute(key, strType, H5::DataSpace())
            .write(strType, value);
    }
    group.close();
}

void SimulationDataWriter::storeSimulationRecords(
    const std::string &group_name,
    const std::unordered_map<std::string, std::vector<scalar>> &records) {
    H5::Group group = file.openGroup(BASE_GROUP + "/" + group_name);

    std::scoped_lock lock(hdf5_mutex);

    for (const auto &[key, value] : records) {
        std::array<hsize_t, 1> dims = {value.size()};
        H5::DataSpace dataspace(1, dims.data());
        H5::DataSet dataset =
            group.createDataSet(key, H5::PredType::NATIVE_LDOUBLE, dataspace);
        dataset.write(value.data(), H5::PredType::NATIVE_LDOUBLE);

        dataset.close();
    }

    group.close();
}

std::unordered_map<std::string, std::string>
SimulationDataWriter::queryMetadata(const std::string &group_name) {
    std::unordered_map<std::string, std::string> metadata;
    H5::Group group = file.openGroup(BASE_GROUP + "/" + group_name);

    std::scoped_lock lock(hdf5_mutex);

    for (int i = 0; i < group.getNumAttrs(); ++i) {
        H5std_string attr_name;
        H5::Attribute attr = group.openAttribute(i);
        attr.getName(attr_name);
        H5::StrType str_type = attr.getStrType();
        std::vector<char> value(attr.getStorageSize());
        attr.read(str_type, value.data());
        metadata[attr_name] = std::string(value.data());
    }

    group.close();
    return metadata;
}