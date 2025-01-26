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

Simulation &Simulation::operator=(const Simulation &other) {
    this->type = other.type;
    strncpy(this->name, other.name, sizeof(this->name));
    this->orbit = other.orbit;
    this->delta_psi_mode = other.delta_psi_mode;
    this->revolutions = other.revolutions;
    this->time_interval = other.time_interval;
    this->record_interval = other.record_interval;

    return *this;
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