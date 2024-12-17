#include <stdexcept>

#include "hdf5/SimulationBuffer.hpp"

SimulationBuffer::SimulationBuffer(const std::vector<std::string> &fields) {
    for (const auto &field : fields) {
        field_data[field] = std::vector<scalar>();
    }
}

void SimulationBuffer::appendData(
    const std::unordered_map<std::string, scalar> &data) {
    for (const auto &[key, value] : data) {
        if (!field_data.contains(key)) {
            throw std::invalid_argument("Field not found in buffer");
        }

        field_data[key].push_back(value);
    }
}

std::vector<scalar>
SimulationBuffer::getFieldData(const std::string &field) const {
    if (!field_data.contains(field)) {
        throw std::invalid_argument("Field not found in buffer");
    }

    return field_data.at(field);
}

std::unordered_map<std::string, std::vector<scalar>>
SimulationBuffer::getAllData() const {
    return field_data;
}

void SimulationBuffer::clearData() {
    for (auto &[key, value] : field_data) {
        value.clear();
    }
}