#include "simulation/model/Simulation.hpp"

namespace simulation::model {

using namespace physics::math;

Simulation::Simulation() = default;

Simulation::Simulation(std::string name, size_t id, bool isRelativistic,
                       bool isQuantized, bool is3D, Vector3 r0, Vector3 v0,
                       Potential potential, scalar delta_time,
                       uint record_interval, scalar total_duration,
                       int rLocalMaxCount)
    : name(std::move(name)), id(id), r0(r0), v0(v0), delta_time(delta_time),
      total_duration(total_duration), rLocalMaxCountLimit(rLocalMaxCount),
      record_interval(record_interval), potential(std::move(potential)),
      isRelativistic(isRelativistic), isQuantized(isQuantized), is3D(is3D),
      status(SimulationStatus::READY) {}

Simulation::Simulation(const Simulation &other)
    : name(other.name), id(other.id) {
    r0 = other.r0;
    v0 = other.v0;
    potential = other.potential;
    status = other.status;
    total_duration = other.total_duration;
    delta_time = other.delta_time;
    record_interval = other.record_interval;
    constantValues = other.constantValues;
    isRelativistic = other.isRelativistic;
    isQuantized = other.isQuantized;
    is3D = other.is3D;
    rLocalMaxCountLimit = other.rLocalMaxCountLimit;
}

Simulation &Simulation::operator=(const Simulation &other) {
    if (this != &other) {
        name = other.name;
        id = other.id;
        potential = other.potential;
        status = other.status;
        r0 = other.r0;
        v0 = other.v0;
        delta_time = other.delta_time;
        total_duration = other.total_duration;
        record_interval = other.record_interval;
        constantValues = other.constantValues;
        isRelativistic = other.isRelativistic;
        isQuantized = other.isQuantized;
        is3D = other.is3D;
        rLocalMaxCountLimit = other.rLocalMaxCountLimit;
    }
    return *this;
}

Simulation::Simulation(Simulation &&other) noexcept
    : name(std::move(other.name)), id(other.id), r0(std::move(other.r0)),
      v0(std::move(other.v0)), delta_time(other.delta_time),
      total_duration(other.total_duration),
      rLocalMaxCountLimit(other.rLocalMaxCountLimit),
      record_interval(other.record_interval),
      potential(std::move(other.potential)),
      constantValues(std::move(other.constantValues)),
      isRelativistic(other.isRelativistic), isQuantized(other.isQuantized),
      is3D(other.is3D), status(other.status) {}

Simulation &Simulation::operator=(Simulation &&other) noexcept {
    if (this != &other) {
        name = std::move(other.name);
        id = other.id;
        r0 = std::move(other.r0);
        v0 = std::move(other.v0);
        potential = std::move(other.potential);
        status = other.status;
        total_duration = other.total_duration;
        delta_time = other.delta_time;
        record_interval = other.record_interval;
        constantValues = std::move(other.constantValues);
        other.id = id;
        is3D = other.is3D;
        isRelativistic = other.isRelativistic;
        isQuantized = other.isQuantized;
        rLocalMaxCountLimit = other.rLocalMaxCountLimit;
    }
    return *this;
}

const std::unordered_map<std::string, scalar> &
Simulation::getConstantValues() const {
    return constantValues;
}

void Simulation::setConstantValues(
    const std::unordered_map<std::string, scalar> &values) {
    constantValues = values;
}

void Simulation::setConstantValues(const std::string &jsonString) {
    nlohmann::json constantsJson = nlohmann::json::parse(jsonString);
    for (const auto &[key, value] : constantsJson.items()) {
        constantValues[key] = value.get<scalar>();
    }
}

void Simulation::clearConstantValues() { constantValues.clear(); }

bool Simulation::hasConstantValue(const std::string &key) const {
    return constantValues.find(key) != constantValues.end();
}

void Simulation::removeConstantValue(const std::string &key) {
    constantValues.erase(key);
}

void Simulation::setConstantValue(const std::string &key, scalar value) {
    constantValues[key] = value;
}

scalar Simulation::getConstantValue(const std::string &key) const {
    auto it = constantValues.find(key);
    if (it != constantValues.end()) {
        return it->second;
    }
    return -9999e99;
}

const Vector3 &Simulation::getR0() const { return r0; }
const Vector3 &Simulation::getV0() const { return v0; }
void Simulation::setR0(const Vector3 &r0) { this->r0 = r0; }
void Simulation::setV0(const Vector3 &v0) { this->v0 = v0; }

Simulation::SimulationStatus Simulation::getStatus() const { return status; }
void Simulation::setName(const std::string &name) { this->name = name; }
void Simulation::setId(size_t id) { this->id = id; }
void Simulation::setStatus(SimulationStatus status) { this->status = status; }
std::string Simulation::getName() const { return name; }
size_t Simulation::getId() const { return id; }

const Potential &Simulation::getPotential() const { return potential; }

void Simulation::setPotential(const Potential &potential) {
    this->potential = potential;
    this->clearConstantValues();
    this->setConstantValues(potential.getConstants());
}

std::string Simulation::serializeConstants() const {
    nlohmann::json constantsJson;
    for (const auto &[key, value] : constantValues) {
        constantsJson[key] = value;
    }
    return constantsJson.dump();
}

scalar Simulation::getDeltaTime() const { return delta_time; }
scalar Simulation::getTotalDuration() const { return total_duration; }
uint Simulation::getRecordInterval() const { return record_interval; }

void Simulation::setDeltaTime(scalar delta_time) {
    this->delta_time = delta_time;
}
void Simulation::setTotalDuration(scalar total_duration) {
    this->total_duration = total_duration;
}
void Simulation::setRecordInterval(uint record_interval) {
    this->record_interval = record_interval;
}

int Simulation::getRLocalMaxCountLimit() const { return rLocalMaxCountLimit; }
void Simulation::setRLocalMaxCountLimit(int count) {
    rLocalMaxCountLimit = count;
}

bool Simulation::getIsRelativistic() const { return isRelativistic; }
bool Simulation::getIsQuantized() const { return isQuantized; }
bool Simulation::getIs3D() const { return is3D; }

void Simulation::setRelativistic(bool is_rel) { isRelativistic = is_rel; }
void Simulation::setQuantized(bool is_quant) { isQuantized = is_quant; }
void Simulation::set3D(bool is_3d) { is3D = is_3d; }

} // namespace simulation::model
