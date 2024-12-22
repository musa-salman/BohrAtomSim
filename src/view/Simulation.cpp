#include <cstring>

#include "view/Simulation.hpp"

Simulation::Simulation(const std::string &name, int type,
                       const std::vector<std::string> &fields)
    : type(type) {
    for (const auto &field : fields) {
        data = std::make_shared<
            std::unordered_map<std::string, std::vector<scalar>>>();

        data->emplace(field, std::vector<scalar>());
    }

    strncpy(this->name, name.c_str(), sizeof(this->name));
}

void Simulation::appendData(
    const std::unordered_map<std::string, scalar> &data) const {
    for (const auto &[key, value] : data) {
        this->data->at(key).push_back(value);
    }
}

std::shared_ptr<std::unordered_map<std::string, std::vector<scalar>>>
Simulation::getData() const {
    return data;
}